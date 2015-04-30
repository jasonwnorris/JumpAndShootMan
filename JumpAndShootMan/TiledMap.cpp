// TiledMap.cpp

// SDL Includes
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
// Project Includes
#include "Globals.hpp"
#include "TiledMap.hpp"
// JSON
#include <json\json.h>
// STL Includes
#include <cstdlib>
#include <fstream>
#include <iostream>

TiledMap::TiledMap()
{
	mIsLoaded = false;
}

TiledMap::~TiledMap()
{
	// Free map data.
	for (int x = 0; x < mWidth; ++x)
	{
		if (mData[x])
		{
			delete [] mData[x];
		}
	}
	if (mData)
	{
		delete [] mData;
	}

	mBitMasks.clear();
}

bool TiledMap::Load(const std::string& pFilename)
{
	if (mIsLoaded)
		return false;

	std::ifstream file;
	file.open(pFilename);
	if (!file.is_open())
		return false;

	std::string tilesetFilename, bitmaskFilename;
	int bitWidth, bitHeight, bitSize;

	std::string ext = pFilename.substr(pFilename.find_last_of('.') + 1);
	if (ext == "txt")
	{
		file >> mWidth >> mHeight;
		file >> tilesetFilename;
		file >> mTileset.X >> mTileset.Y >> mTileset.Size;
		file >> bitmaskFilename;
		file >> bitWidth >> bitHeight >> bitSize;

		mIsLoaded = true;
	}
	else if (ext == "json")
	{
		Json::Value root;
		Json::Reader reader;

		// "Tiled" editor output
		if (reader.parse(file, root, false))
		{
			mWidth = root["width"].asInt();
			mHeight = root["height"].asInt();

			const Json::Value tilesets = root["tilesets"];
			for (Json::Value::UInt i = 0; i < tilesets.size(); ++i)
			{
				std::string name = tilesets[i]["name"].asString();
				std::string path = tilesets[i]["image"].asString();
				int imageWidth = tilesets[i]["imagewidth"].asInt();
				int imageHeight = tilesets[i]["imageheight"].asInt();
				int tileSize = tilesets[i]["tilewidth"].asInt();

				if (name == "Texture")
				{
					tilesetFilename = path;
					mTileset.X = imageWidth / tileSize;
					mTileset.Y = imageHeight / tileSize;
					mTileset.Size = tileSize;
				}
				else if (name == "Bitmask")
				{
					bitmaskFilename = path;
					bitWidth = imageWidth / tileSize;
					bitHeight = imageHeight / tileSize;
					bitSize = tileSize;
				}
			}

			mIsLoaded = true;
		}
		else
		{
			SDL_Log("[TiledMap::Load] Failed to parse JSON file.");
		}
	}
	else
	{
		SDL_Log("[TiledMap::Load] Unsupported map file format: %s", ext);
	}

	// Load tileset texture;
	if (!mTileset.Texture.Load(tilesetFilename.c_str(), HGF::Interpolation::Nearest, HGF::Wrapping::ClampToBorder))
		return false;

	// Load bitmask texture.
	HGF::Texture bitmaskTexture;
	if (!bitmaskTexture.Load(bitmaskFilename.c_str()))
		return false;

	// Establish bitmask count.
	mBitMaskCount = bitWidth * bitHeight;
	mBitMasks.resize(mBitMaskCount);

	// Read the bitmask pixels.
	for (int i = 0; i < mBitMaskCount; ++i)
	{
		mBitMasks[i] = std::make_unique<BitField>();
		mBitMasks[i].get()->Resize(mTileset.Size, mTileset.Size);

		for (int px = 0; px < mTileset.Size; ++px)
		{
			for (int py = 0; py < mTileset.Size; ++py)
			{
				Uint8 r, g, b, a;
				bitmaskTexture.GetColor(i * mTileset.Size + px, py, r, g, b, a);

				mBitMasks[i].get()->SetBit(px, py, (r == 0 && g == 0 && b == 0));
			}
		}
	}

	// Allocate tile memory.
	mData = new Tile*[mWidth];
	for (int x = 0; x < mWidth; ++x)
	{
		mData[x] = new Tile[mHeight];
	}

	// Load tile values.
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			Tile& tile = mData[x][y];
			int orientation;

			file >> tile.TextureID >> tile.CollisionID >> orientation;

			// Assign orientation. (This could be cleaner.)
			switch (orientation)
			{
			default:
			case 1:
				tile.Orientation = SAGE::Orientation::None;
				break;
			case 2:
				tile.Orientation = SAGE::Orientation::FlipHorizontal;
				break;
			case 4:
				tile.Orientation = SAGE::Orientation::FlipVertical;
				break;
			case 6:
				tile.Orientation = SAGE::Orientation::FlipBoth;
				break;
			}

			// Assign edge type depending on collision. (Temporary)
			switch (tile.CollisionID)
			{
			case 0:
				for (int i = 0; i < 4; ++i)
					tile.Edges[i] = EdgeType::Solid;
				break;
			case 1:
			case 3:
				tile.Edges[Direction::Up] = EdgeType::Interesting;
				tile.Edges[Direction::Down] = EdgeType::Solid;
				tile.Edges[Direction::Left] = EdgeType::Interesting;
				tile.Edges[Direction::Right] = EdgeType::Solid;
				break;
			case 2:
			case 4:
				tile.Edges[Direction::Up] = EdgeType::Interesting;
				tile.Edges[Direction::Down] = EdgeType::Solid;
				tile.Edges[Direction::Left] = EdgeType::Interesting;
				tile.Edges[Direction::Right] = EdgeType::Interesting;
				break;
			default:
				for (int i = 0; i < 4; ++i)
					tile.Edges[i] = EdgeType::Empty;
				break;
			}

			// Flip edges for orientation.
			if ((tile.Orientation & SAGE::Orientation::FlipVertical) == SAGE::Orientation::FlipVertical)
				std::swap(tile.Edges[Direction::Up], tile.Edges[Direction::Down]);
			if ((tile.Orientation & SAGE::Orientation::FlipHorizontal) == SAGE::Orientation::FlipHorizontal)
				std::swap(tile.Edges[Direction::Left], tile.Edges[Direction::Right]);
		}
	}

	// Resolve collision edges.
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			Tile& tile = mData[x][y];
			Tile& tileHorz = mData[x + 1][y];
			Tile& tileVert = mData[x][y + 1];

			if (x + 1 != mWidth)
			{
				if ((tile.Edges[Direction::Right] == EdgeType::Solid || tile.Edges[Direction::Right] == EdgeType::Interesting) &&
					(tileHorz.Edges[Direction::Left] == EdgeType::Solid || tileHorz.Edges[Direction::Left] == EdgeType::Interesting))
				{
					tile.Edges[Direction::Right] = EdgeType::Empty;
					tileHorz.Edges[Direction::Left] = EdgeType::Empty;
				}
			}
			if (y + 1 != mHeight)
			{
				if ((tile.Edges[Direction::Down] == EdgeType::Solid || tile.Edges[Direction::Down] == EdgeType::Interesting) &&
					(tileVert.Edges[Direction::Up] == EdgeType::Solid || tileVert.Edges[Direction::Up] == EdgeType::Interesting))
				{
					tile.Edges[Direction::Down] = EdgeType::Empty;
					tileVert.Edges[Direction::Up] = EdgeType::Empty;
				}
			}
		}
	}

	file.close();

	return mIsLoaded;
}

bool TiledMap::TryGetTile(int pX, int pY, Tile& pTile) const
{
	if (pX < 0 || pY < 0 || pX >= mWidth || pY >= mHeight)
		return false;

	pTile = mData[pX][pY];

	return pTile.TextureID >= 0;
}

bool TiledMap::IsEnterable(const Tile& pTile, Direction pDirection, bool pHasInterest) const
{
	return pTile.Edges[OppositeDirection(pDirection)] == EdgeType::Solid || (pHasInterest && pTile.Edges[OppositeDirection(pDirection)] == EdgeType::Interesting);
}

bool TiledMap::IsTraversable(const Tile& pTile, int pPixelX, int pPixelY) const
{
	if (pPixelX < 0 || pPixelY < 0 || pPixelX >= mTileset.Size || pPixelY >= mTileset.Size)
		return true;

	if (pTile.CollisionID < 0)
		return true;

	bool horzFlip = (pTile.Orientation & SAGE::Orientation::FlipHorizontal) == SAGE::Orientation::FlipHorizontal;
	bool vertFlip = (pTile.Orientation & SAGE::Orientation::FlipVertical) == SAGE::Orientation::FlipVertical;

	return !mBitMasks[pTile.CollisionID].get()->GetBit(horzFlip ? mTileset.Size - pPixelX : pPixelX, vertFlip ? mTileset.Size - pPixelY : pPixelY);
}

void TiledMap::Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit) const
{
	int x = (int)std::roundf(pPosition.X);
	int y = (int)std::roundf(pPosition.Y);
	int tileX = x / mTileset.Size;
	int tileY = y / mTileset.Size;
	int pixelX = x % mTileset.Size;
	int pixelY = y % mTileset.Size;

	float maxDistance = 1000.0f;
	float left = 0.0f;
	float top = 0.0f;
	float right = (float)(mWidth * mTileset.Size);
	float bottom = (float)(mHeight * mTileset.Size);

	pRaycastHit.Position = HGF::Vector2::Zero;
	pRaycastHit.Distance = maxDistance;

	if (pPosition.X < left || pPosition.X > right || pPosition.Y < top || pPosition.Y > bottom || x < 0 || x >= mWidth * mTileset.Size || y < 0 || y >= mHeight * mTileset.Size)
	{
		switch (pDirection)
		{
			case Direction::Up:
				pRaycastHit.Position = HGF::Vector2(pPosition.X, pPosition.Y - maxDistance);
				break;
			case Direction::Down:
				pRaycastHit.Position = HGF::Vector2(pPosition.X, pPosition.Y + maxDistance);
				break;
			case Direction::Left:
				pRaycastHit.Position = HGF::Vector2(pPosition.X - maxDistance, pPosition.Y);
				break;
			case Direction::Right:
				pRaycastHit.Position = HGF::Vector2(pPosition.X + maxDistance, pPosition.Y);
				break;
		}
		return;
	}

	Tile tile;
	switch (pDirection)
	{
		case Direction::Up:
			for (int ty = tileY; ty >= 0; --ty)
			{
				if (TryGetTile(tileX, ty, tile))
				{
					if (IsEnterable(tile, pDirection, pHasInterest))
					{
						for (int py = mTileset.Size - 1; py >= 0; --py)
						{
							if (!IsTraversable(tile, pixelX, py))
							{
								pRaycastHit.Position = HGF::Vector2(pPosition.X, (float)(ty * mTileset.Size + py));
								pRaycastHit.Distance = pPosition.Y - pRaycastHit.Position.Y;
								pRaycastHit.TileX = tileX;
								pRaycastHit.TileY = ty;
								return;
							}
						}
					}
				}
			}
			pRaycastHit.Position = HGF::Vector2(pPosition.X, top);
			pRaycastHit.Distance = pPosition.Y - top;
			pRaycastHit.TileX = tileX;
			pRaycastHit.TileY = 0;
			break;
		case Direction::Down:
			for (int ty = tileY; ty < mHeight; ++ty)
			{
				if (TryGetTile(tileX, ty, tile))
				{
					if (IsEnterable(tile, pDirection, pHasInterest))
					{
						for (int py = 0; py < mTileset.Size; ++py)
						{
							if (!IsTraversable(tile, pixelX, py))
							{
								pRaycastHit.Position = HGF::Vector2(pPosition.X, (float)(ty * mTileset.Size + py));
								pRaycastHit.Distance = pRaycastHit.Position.Y - pPosition.Y;
								pRaycastHit.TileX = tileX;
								pRaycastHit.TileY = ty;
								return;
							}
						}
					}
				}
			}
			pRaycastHit.Position = HGF::Vector2(pPosition.X, bottom);
			pRaycastHit.Distance = bottom - pPosition.Y;
			pRaycastHit.TileX = tileX;
			pRaycastHit.TileY = mHeight - 1;
			break;
		case Direction::Left:
			for (int tx = tileX; tx >= 0; --tx)
			{
				if (TryGetTile(tx, tileY, tile))
				{
					if (IsEnterable(tile, pDirection, pHasInterest))
					{
						for (int px = mTileset.Size - 1; px >= 0; --px)
						{
							if (!IsTraversable(tile, px, pixelY))
							{
								pRaycastHit.Position = HGF::Vector2((float)(tx * mTileset.Size + px), pPosition.Y);
								pRaycastHit.Distance = pPosition.X - pRaycastHit.Position.X;
								pRaycastHit.TileX = tileY;
								pRaycastHit.TileY = tx;
								return;
							}
						}
					}
				}
			}
			pRaycastHit.Position = HGF::Vector2(left, pPosition.Y);
			pRaycastHit.Distance = pPosition.X - left;
			pRaycastHit.TileX = 0;
			pRaycastHit.TileY = tileY;
			break;
		case Direction::Right:
			for (int tx = tileX; tx < mWidth; ++tx)
			{
				if (TryGetTile(tx, tileY, tile))
				{
					if (IsEnterable(tile, pDirection, pHasInterest))
					{
						for (int px = 0; px < mTileset.Size; ++px)
						{
							if (!IsTraversable(tile, px, pixelY))
							{
								pRaycastHit.Position = HGF::Vector2((float)(tx * mTileset.Size + px), pPosition.Y);
								pRaycastHit.Distance = pRaycastHit.Position.X - pPosition.X;
								pRaycastHit.TileX = tileY;
								pRaycastHit.TileY = tx;
								return;
							}
						}
					}
				}
			}
			pRaycastHit.Position = HGF::Vector2(right, pPosition.Y);
			pRaycastHit.Distance = right - pPosition.X;
			pRaycastHit.TileX = mWidth - 1;
			pRaycastHit.TileY = tileY;
			break;
		default:
			// what?
			break;
	}
}

void TiledMap::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	Tile tile;
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (TryGetTile(x, y, tile))
			{
				HGF::Vector2 position(x * mTileset.Size, y * mTileset.Size);

				HGF::Rectangle source;
				source.X = (tile.TextureID % mTileset.X) * mTileset.Size;
				source.Y = (tile.TextureID / mTileset.X)  * mTileset.Size;
				source.Width = mTileset.Size;
				source.Height = mTileset.Size;

				pSpriteBatch.Draw(mTileset.Texture, position, source, HGF::Color::White, HGF::Vector2::Zero, 0.0f, HGF::Vector2::One, tile.Orientation);
			}
		}
	}
}

void TiledMap::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	Tile tile;
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (TryGetTile(x, y, tile))
			{
				for (int i = 0; i < 4; ++i)
				{
					if (tile.Edges[i] == EdgeType::Solid || tile.Edges[i] == EdgeType::Interesting)
					{
						HGF::Color color(tile.Edges[i] == EdgeType::Interesting ? 0.0f : 0.8f, 0.0f, tile.Edges[i] == EdgeType::Interesting ? 0.8f : 0.0f);

						switch (i)
						{
						case Direction::Up:
							pGeometryBatch.Draw(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), color);
							break;
						case Direction::Down:
							pGeometryBatch.Draw(HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), color);
							break;
						case Direction::Left:
							pGeometryBatch.Draw(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), color);
							break;
						case Direction::Right:
							pGeometryBatch.Draw(HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), color);
							break;
						}
					}
				}
			}
		}
	}
}