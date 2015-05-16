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
						tile.Edges[i] = EdgeMask::Solid;
					break;
				case 1:
				case 3:
					tile.Edges[EdgeSide::Top] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Bottom] = EdgeMask::Solid;
					tile.Edges[EdgeSide::Left] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Right] = EdgeMask::Solid;
					break;
				case 2:
				case 4:
					tile.Edges[EdgeSide::Top] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Bottom] = EdgeMask::Solid;
					tile.Edges[EdgeSide::Left] = EdgeMask::Interesting;
					tile.Edges[EdgeSide::Right] = EdgeMask::Interesting;
					break;
				default:
					for (int i = 0; i < 4; ++i)
						tile.Edges[i] = EdgeMask::Empty;
					break;
				}

				// Flip edges for orientation.
				if ((tile.Orientation & SAGE::Orientation::FlipVertical) == SAGE::Orientation::FlipVertical)
					std::swap(tile.Edges[EdgeSide::Top], tile.Edges[EdgeSide::Bottom]);
				if ((tile.Orientation & SAGE::Orientation::FlipHorizontal) == SAGE::Orientation::FlipHorizontal)
					std::swap(tile.Edges[EdgeSide::Left], tile.Edges[EdgeSide::Right]);
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

					if ((tile.Edges[EdgeSide::Right] == EdgeMask::Solid || tile.Edges[EdgeSide::Right] == EdgeMask::Interesting) &&
						(tileHorz.Edges[EdgeSide::Left] == EdgeMask::Solid || tileHorz.Edges[EdgeSide::Left] == EdgeMask::Interesting))
					{
						tile.Edges[EdgeSide::Right] = EdgeMask::Empty;
						tileHorz.Edges[EdgeSide::Left] = EdgeMask::Empty;
					}
				}
				if (y + 1 != mHeight)
				{
					Tile& tileVert = mData[x][y + 1];

					if ((tile.Edges[EdgeSide::Bottom] == EdgeMask::Solid || tile.Edges[EdgeSide::Bottom] == EdgeMask::Interesting) &&
						(tileVert.Edges[EdgeSide::Top] == EdgeMask::Solid || tileVert.Edges[EdgeSide::Top] == EdgeMask::Interesting))
					{
						tile.Edges[EdgeSide::Bottom] = EdgeMask::Empty;
						tileVert.Edges[EdgeSide::Top] = EdgeMask::Empty;
					}
				}
			}
		}
	}

	return isLoaded;
}

void TiledMap::Raycast(const HGF::Vector2& pPosition, const HGF::Vector2& pDirection, int pLayerMasks, RaycastData& pRaycastData) const
{
	int x = (int)std::roundf(pPosition.X);
	int y = (int)std::roundf(pPosition.Y);
	int tileX = x / mTileset.Size;
	int tileY = y / mTileset.Size;
	int pixelX = x % mTileset.Size;
	int pixelY = y % mTileset.Size;

	float top = 0.0f;
	float bottom = (float)(mHeight * mTileset.Size);
	float right = (float)(mWidth * mTileset.Size);
	float left = 0.0f;

	pRaycastData.StartPosition = pPosition;
	pRaycastData.Direction = pDirection;

	// Capture outside cases.
	if ((pDirection == HGF::Vector2::Up && (pPosition.Y < top || pPosition.X < left || pPosition.X > right)) ||
		(pDirection == HGF::Vector2::Down && (pPosition.Y > bottom || pPosition.X < left || pPosition.X > right)) ||
		(pDirection == HGF::Vector2::Left && (pPosition.X < left || pPosition.Y < top || pPosition.Y > bottom)) ||
		(pDirection == HGF::Vector2::Right && (pPosition.X > right || pPosition.Y < top || pPosition.Y > bottom)))
	{
		pRaycastData.IsValid = false;
		return;
	}

	Tile tile;
	if (pDirection == HGF::Vector2::Up)
	{
		for (int ty = tileY; ty >= 0; --ty)
		{
			if (TryGetTile(tileX, ty, tile))
			{
				if (IsEnterable(tile, EdgeSide::Bottom, pLayerMasks))
				{
					for (int py = mTileset.Size - 1; py >= 0; --py)
					{
						if (!IsTraversable(tile, pixelX, py))
						{
							pRaycastData.EndPosition = HGF::Vector2(pPosition.X, (float)(ty * mTileset.Size + py));
							pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
							return;
						}
					}
				}
			}
		}
		pRaycastData.EndPosition = HGF::Vector2(pPosition.X, top);
		pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
	}
	else if (pDirection == HGF::Vector2::Down)
	{
		for (int ty = tileY; ty < mHeight; ++ty)
		{
			if (TryGetTile(tileX, ty, tile))
			{
				if (IsEnterable(tile, EdgeSide::Top, pLayerMasks))
				{
					for (int py = 0; py < mTileset.Size; ++py)
					{
						if (!IsTraversable(tile, pixelX, py))
						{
							pRaycastData.EndPosition = HGF::Vector2(pPosition.X, (float)(ty * mTileset.Size + py));
							pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
							return;
						}
					}
				}
			}
		}
		pRaycastData.EndPosition = HGF::Vector2(pPosition.X, bottom);
		pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
	}
	else if (pDirection == HGF::Vector2::Left)
	{
		for (int tx = tileX; tx >= 0; --tx)
		{
			if (TryGetTile(tx, tileY, tile))
			{
				if (IsEnterable(tile, EdgeSide::Right, pLayerMasks))
				{
					for (int px = mTileset.Size - 1; px >= 0; --px)
					{
						if (!IsTraversable(tile, px, pixelY))
						{
							pRaycastData.EndPosition = HGF::Vector2((float)(tx * mTileset.Size + px), pPosition.Y);
							pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
							return;

						}
					}
				}
			}
		}
		pRaycastData.EndPosition = HGF::Vector2(left, pPosition.Y);
		pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
		pRaycastData.IsValid = false;
	}
	else if (pDirection == HGF::Vector2::Right)
	{
		for (int tx = tileX; tx < mWidth; ++tx)
		{
			if (TryGetTile(tx, tileY, tile))
			{
				if (IsEnterable(tile, EdgeSide::Left, pLayerMasks))
				{
					for (int px = 0; px < mTileset.Size; ++px)
					{
						if (!IsTraversable(tile, px, pixelY))
						{
							pRaycastData.EndPosition = HGF::Vector2((float)(tx * mTileset.Size + px), pPosition.Y);
							pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
							return;
						}
					}
				}
			}
		}
		pRaycastData.EndPosition = HGF::Vector2(right, pPosition.Y);
		pRaycastData.Distance = HGF::Vector2::Distance(pRaycastData.StartPosition, pRaycastData.EndPosition);
	}
}

void TiledMap::Update(float pDeltaTime)
{
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
					if (tile.Edges[i] != EdgeMask::Empty)
					{
						HGF::Color color(tile.Edges[i] == EdgeMask::Interesting ? 0.0f : 0.8f, 0.0f, tile.Edges[i] == EdgeMask::Interesting ? 0.8f : 0.0f);

						switch (i)
						{
							case EdgeSide::Top:
								pGeometryBatch.DrawLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), color);
								break;
							case EdgeSide::Bottom:
								pGeometryBatch.DrawLine(HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), color);
								break;
							case EdgeSide::Left:
								pGeometryBatch.DrawLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), color);
								break;
							case EdgeSide::Right:
								pGeometryBatch.DrawLine(HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), color);
								break;
						}
					}
				}
			}
		}
	}
}

bool TiledMap::TryGetTile(int pX, int pY, Tile& pTile) const
{
	if (pX < 0 || pY < 0 || pX >= mWidth || pY >= mHeight)
		return false;

	pTile = mData[pX][pY];

	return pTile.TextureID >= 0;
}

bool TiledMap::IsEnterable(const Tile& pTile, EdgeSide pEdge, int pMask) const
{
	if (pMask == EdgeMask::Interesting)
	{
		return pTile.Edges[pEdge] == EdgeMask::Solid || pTile.Edges[pEdge] == EdgeMask::Interesting;
	}
	else if (pMask == EdgeMask::Solid)
	{
		return pTile.Edges[pEdge] == EdgeMask::Solid;
	}	
}

bool TiledMap::IsTraversable(const Tile& pTile, int pPixelX, int pPixelY) const
{
	if (pPixelX < 0 || pPixelY < 0 || pPixelX >= mTileset.Size || pPixelY >= mTileset.Size)
		return true;

	if (pTile.CollisionID < 0)
		return true;

	bool horzFlip = (pTile.Orientation & SAGE::Orientation::FlipHorizontal) == SAGE::Orientation::FlipHorizontal;
	bool vertFlip = (pTile.Orientation & SAGE::Orientation::FlipVertical) == SAGE::Orientation::FlipVertical;

	return !mBitMasks[pTile.CollisionID].GetBit(horzFlip ? mTileset.Size - pPixelX - 1 : pPixelX, vertFlip ? mTileset.Size - pPixelY - 1 : pPixelY);
}