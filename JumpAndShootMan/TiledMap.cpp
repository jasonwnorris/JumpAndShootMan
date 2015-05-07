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
}

TiledMap::~TiledMap()
{
	mData.clear();
	mBitMasks.clear();
}

bool TiledMap::Load(const std::string& pFilename)
{
	// Open the file.
	std::ifstream file;
	file.open(pFilename);
	if (!file.is_open())
		return false;

	std::string tilesetFilename, bitmaskFilename;
	int bitWidth, bitHeight, bitSize;
	bool isLoaded = false;

	// Check file extension.
	std::string ext = pFilename.substr(pFilename.find_last_of('.') + 1);
	if (ext == "txt")
	{
		int version;

		file >> version;
		file >> mWidth >> mHeight;
		file >> tilesetFilename;
		file >> mTileset.X >> mTileset.Y >> mTileset.Size;
		file >> bitmaskFilename;
		file >> bitWidth >> bitHeight >> bitSize;

		mData.resize(mWidth, std::vector<Tile>(mHeight));

		// Load tile values.
		for (int y = 0; y < mHeight; ++y)
		{
			for (int x = 0; x < mWidth; ++x)
			{
				int tex, col, ori;
				file >> tex >> col >> ori;

				Tile& tile = mData[x][y];
				tile.TextureID = tex;
				tile.CollisionID = col;
				tile.Orientation = static_cast<SAGE::Orientation>(ori);
			}
		}

		isLoaded = true;
	}
	else if (ext == "json")
	{
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(file, root, false))
		{
			int version = root["version"].asInt();
			mWidth = root["width"].asInt();
			mHeight = root["height"].asInt();
			const Json::Value tileset = root["tileset"];
			const Json::Value bitset = root["bitset"];
			const Json::Value data = root["data"];

			mTileset.X = tileset.get("x", 0).asInt();
			mTileset.Y = tileset.get("y", 0).asInt();
			mTileset.Size = tileset.get("size", 0).asInt();
			tilesetFilename = tileset.get("path", "").asString();

			bitWidth = bitset.get("x", 0).asInt();
			bitHeight = bitset.get("y", 0).asInt();
			bitSize = bitset.get("size", 0).asInt();
			bitmaskFilename = bitset.get("path", "").asString();

			mData.resize(mWidth, std::vector<Tile>(mHeight));

			// Load tile values.
			for (int y = 0; y < mHeight; ++y)
			{
				const Json::Value row = data[y];

				for (int x = 0; x < mWidth; ++x)
				{
					const Json::Value item = row[x];
					int index = 0;

					Tile& tile = mData[x][y];
					tile.TextureID = item[index++].asInt();
					tile.CollisionID = item[index++].asInt();
					tile.Orientation = static_cast<SAGE::Orientation>(item[index].asInt());
				}
			}

			isLoaded = true;
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

	file.close();

	if (isLoaded)
	{
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
			mBitMasks[i].Resize(mTileset.Size, mTileset.Size);

			for (int px = 0; px < mTileset.Size; ++px)
			{
				for (int py = 0; py < mTileset.Size; ++py)
				{
					Uint8 r, g, b, a;
					bitmaskTexture.GetColor(i * mTileset.Size + px, py, r, g, b, a);

					mBitMasks[i].SetBit(px, py, (r == 0 && g == 0 && b == 0));
				}
			}
		}

		// Assign attibutes from values.
		for (int y = 0; y < mHeight; ++y)
		{
			for (int x = 0; x < mWidth; ++x)
			{
				Tile& tile = mData[x][y];

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

				if (x + 1 != mWidth)
				{
					Tile& tileHorz = mData[x + 1][y];

					if ((tile.Edges[Direction::Right] == EdgeType::Solid || tile.Edges[Direction::Right] == EdgeType::Interesting) &&
						(tileHorz.Edges[Direction::Left] == EdgeType::Solid || tileHorz.Edges[Direction::Left] == EdgeType::Interesting))
					{
						tile.Edges[Direction::Right] = EdgeType::Empty;
						tileHorz.Edges[Direction::Left] = EdgeType::Empty;
					}
				}
				if (y + 1 != mHeight)
				{
					Tile& tileVert = mData[x][y + 1];

					if ((tile.Edges[Direction::Down] == EdgeType::Solid || tile.Edges[Direction::Down] == EdgeType::Interesting) &&
						(tileVert.Edges[Direction::Up] == EdgeType::Solid || tileVert.Edges[Direction::Up] == EdgeType::Interesting))
					{
						tile.Edges[Direction::Down] = EdgeType::Empty;
						tileVert.Edges[Direction::Up] = EdgeType::Empty;
					}
				}
			}
		}
	}

	return isLoaded;
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

	return !mBitMasks[pTile.CollisionID].GetBit(horzFlip ? mTileset.Size - pPixelX : pPixelX, vertFlip ? mTileset.Size - pPixelY : pPixelY);
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
							pGeometryBatch.DrawLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), color);
							break;
						case Direction::Down:
							pGeometryBatch.DrawLine(HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), color);
							break;
						case Direction::Left:
							pGeometryBatch.DrawLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), color);
							break;
						case Direction::Right:
							pGeometryBatch.DrawLine(HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), color);
							break;
						}
					}
				}
			}
		}
	}
}