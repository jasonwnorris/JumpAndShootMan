#include "Globals.hpp"
#include "Map.hpp"
#include "Renderer.hpp"

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

Map::Map()
{
	mIsLoaded = false;
}

Map::~Map()
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

	// Free map bitmasks.
	for (int i = 0; i < mBitMaskCount; ++i)
	{
		for (int x = 0; x < mTileset.Size; ++x)
		{
			if (mBitMasks[i][x])
			{
				delete [] mBitMasks[i][x];
			}
		}
		if (mBitMasks[i])
		{
			delete [] mBitMasks[i];
		}
	}
	if (mBitMasks)
	{
		delete [] mBitMasks;
	}
}

bool Map::Load(const std::string& pFilename)
{
	if (mIsLoaded)
		return false;

	std::ifstream file;
	file.open(pFilename);
	if (!file.is_open())
		return false;

	std::string tilesetFilename, bitmaskFilename;
	int bitWidth, bitHeight, bitSize;

	file >> mWidth >> mHeight;
	file >> tilesetFilename;
	file >> mTileset.X >> mTileset.Y >> mTileset.Size;
	file >> bitmaskFilename;
	file >> bitWidth >> bitHeight >> bitSize;

	// Load tileset texture;
	if (!mTileset.Texture.Load(tilesetFilename))
		return false;

	// Load bitmask texture.
	HGF::Texture bitmaskTexture;
	if (!bitmaskTexture.Load(bitmaskFilename))
		return false;

	// Establish bitmask count.
	mBitMaskCount = bitWidth * bitHeight;
	mBitMasks = new bool**[mBitMaskCount];

	// Read the bitmask pixels.
	for (int i = 0; i < mBitMaskCount; ++i)
	{
		mBitMasks[i] = new bool*[mTileset.Size];

		for (int px = 0; px < mTileset.Size; ++px)
		{
			mBitMasks[i][px] = new bool[mTileset.Size];

			for (int py = 0; py < mTileset.Size; ++py)
			{
				Uint8 r, g, b, a;
				bitmaskTexture.GetColor(i * mTileset.Size + px, py, r, g, b, a);

				mBitMasks[i][px][py] = (r == 0 && g == 0 && b == 0);
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

			file >> tile.TextureID >> tile.CollisionID;
			
			switch (tile.CollisionID)
			{
				case 0:
					for (int i = 0; i < 4; ++i)
						tile.Edges[i] = EdgeType::Solid;
					break;
				case 1:
					tile.Edges[Direction::Up] = EdgeType::Interesting;
					tile.Edges[Direction::Down] = EdgeType::Solid;
					tile.Edges[Direction::Left] = EdgeType::Interesting;
					tile.Edges[Direction::Right] = EdgeType::Solid;
					break;
				case 2:
					tile.Edges[Direction::Up] = EdgeType::Interesting;
					tile.Edges[Direction::Down] = EdgeType::Solid;
					tile.Edges[Direction::Left] = EdgeType::Solid;
					tile.Edges[Direction::Right] = EdgeType::Interesting;
					break;
				default:
					for (int i = 0; i < 4; ++i)
						tile.Edges[i] = EdgeType::Empty;
					break;
			}
		}
	}

	// Resolve collision edges.
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (x + 1 != mWidth)
			{
				if (mData[x][y].Edges[Direction::Right] == EdgeType::Solid && mData[x + 1][y].Edges[Direction::Left] == EdgeType::Solid)
				{
					mData[x][y].Edges[Direction::Right] = EdgeType::Empty;
					mData[x + 1][y].Edges[Direction::Left] = EdgeType::Empty;
				}
			}
			if (y + 1 != mHeight)
			{
				if (mData[x][y].Edges[Direction::Down] == EdgeType::Solid && mData[x][y + 1].Edges[Direction::Up] == EdgeType::Solid)
				{
					mData[x][y].Edges[Direction::Down] = EdgeType::Empty;
					mData[x][y + 1].Edges[Direction::Up] = EdgeType::Empty;
				}
			}
		}
	}

	file.close();

	mIsLoaded = true;

	return mIsLoaded;
}

void Map::Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit)
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

	switch (pDirection)
	{
		case Direction::Up:
			for (int ty = tileY; ty >= 0; --ty)
			{
				if (mData[tileX][ty].Edges[Direction::Down] == EdgeType::Solid || (pHasInterest && mData[tileX][ty].Edges[Direction::Down] == EdgeType::Interesting))
				{
					for (int py = mTileset.Size - 1; py >= 0; --py)
					{
						if (!IsTraversable(tileX, ty, pixelX, py))
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
			pRaycastHit.Position = HGF::Vector2(pPosition.X, top);
			pRaycastHit.Distance = pPosition.Y - top;
			pRaycastHit.TileX = tileX;
			pRaycastHit.TileY = 0;
			break;
		case Direction::Down:
			for (int ty = tileY; ty < mHeight; ++ty)
			{
				if (mData[tileX][ty].Edges[Direction::Up] == EdgeType::Solid || (pHasInterest && mData[tileX][ty].Edges[Direction::Up] == EdgeType::Interesting))
				{
					for (int py = 0; py < mTileset.Size; ++py)
					{
						if (!IsTraversable(tileX, ty, pixelX, py))
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
			pRaycastHit.Position = HGF::Vector2(pPosition.X, bottom);
			pRaycastHit.Distance = bottom - pPosition.Y;
			pRaycastHit.TileX = tileX;
			pRaycastHit.TileY = mHeight - 1;
			break;
		case Direction::Left:
			for (int tx = tileX; tx >= 0; --tx)
			{
				if (mData[tx][tileY].Edges[Direction::Right] == EdgeType::Solid || (pHasInterest && mData[tx][tileY].Edges[Direction::Right] == EdgeType::Interesting))
				{
					for (int px = mTileset.Size - 1; px >= 0; --px)
					{
						if (!IsTraversable(tx, tileY, px, pixelY))
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
			pRaycastHit.Position = HGF::Vector2(left, pPosition.Y);
			pRaycastHit.Distance = pPosition.X - left;
			pRaycastHit.TileX = 0;
			pRaycastHit.TileY = tileY;
			break;
		case Direction::Right:
			for (int tx = tileX; tx < mWidth; ++tx)
			{
				if (mData[tx][tileY].Edges[Direction::Left] == EdgeType::Solid || (pHasInterest && mData[tx][tileY].Edges[Direction::Left] == EdgeType::Interesting))
				{
					for (int px = 0; px < mTileset.Size; ++px)
					{
						if (!IsTraversable(tx, tileY, px, pixelY))
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

bool Map::IsTileEmpty(int pTileX, int pTileY)
{
	if (pTileX < 0 || pTileY < 0 || pTileX >= mWidth || pTileY >= mHeight)
		return true;

	return mData[pTileX][pTileY].CollisionID < 0;
}

bool Map::IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY)
{
	if (pTileX < 0 || pTileY < 0 || pTileX >= mWidth || pTileY >= mHeight)
		return true;

	if (pPixelX < 0 || pPixelY < 0 || pPixelX >= mTileset.Size || pPixelY >= mTileset.Size)
		return true;

	if (mData[pTileX][pTileY].CollisionID < 0)
		return true;

	return !mBitMasks[mData[pTileX][pTileY].CollisionID][pPixelX][pPixelY];
}

bool Map::Render(const Renderer& pRenderer)
{
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			int value = mData[x][y].TextureID;
			if (value >= 0)
			{
				HGF::Vector2 position(x * mTileset.Size, y * mTileset.Size);
				HGF::Vector2 dimensions(mTileset.Size, mTileset.Size);
				HGF::Vector2 min((value % mTileset.X) * mTileset.Size, (value / mTileset.X)  * mTileset.Size);
				HGF::Vector2 max(((value % mTileset.X) + 1)  * mTileset.Size, ((value / mTileset.X) + 1)  * mTileset.Size);

				if (pRenderer.RenderTexture(mTileset.Texture, position, dimensions, HGF::Vector2::Zero, min, max) < 0)
					return false;

				if (Globals::IsDebugDrawOn)
				{
					for (int i = 0; i < 4; ++i)
					{
						if (mData[x][y].Edges[i] == EdgeType::Solid || mData[x][y].Edges[i] == EdgeType::Interesting)
						{
							float r = mData[x][y].Edges[i] == EdgeType::Interesting ? 0.0f : 0.8f;
							float g = 0.0f;
							float b = mData[x][y].Edges[i] == EdgeType::Interesting ? 0.8f : 0.0f;

							switch (i)
							{
							case Direction::Up:
								if (pRenderer.RenderLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							case Direction::Down:
								if (pRenderer.RenderLine(HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							case Direction::Left:
								if (pRenderer.RenderLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							case Direction::Right:
								if (pRenderer.RenderLine(HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							}
						}
					}
				}
			}
		}
	}

	return true;
}