// Map.cpp

// SDL Includes
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
// Project Includes
#include "Globals.hpp"
#include "Map.hpp"
#include "Renderer.hpp"
// STL Includes
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
	if (!mTileset.Texture.Load(tilesetFilename.c_str()))
		return false;

	// Load bitmask texture.
	HGF::Texture bitmaskTexture;
	if (!bitmaskTexture.Load(bitmaskFilename.c_str()))
		return false;

	// Establish bitmask count.
	mBitMaskCount = bitWidth * bitHeight;
	mBitMasks = new BitField[mBitMaskCount];

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

			tile.Orientation = SAGE::Orientation::None;
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
				if (tile.Edges[Direction::Right] == EdgeType::Solid && tileHorz.Edges[Direction::Left] == EdgeType::Solid)
				{
					tile.Edges[Direction::Right] = EdgeType::Empty;
					tileHorz.Edges[Direction::Left] = EdgeType::Empty;
				}
			}
			if (y + 1 != mHeight)
			{
				if (tile.Edges[Direction::Down] == EdgeType::Solid && tileVert.Edges[Direction::Up] == EdgeType::Solid)
				{
					tile.Edges[Direction::Down] = EdgeType::Empty;
					tileVert.Edges[Direction::Up] = EdgeType::Empty;
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
				Tile& tile = mData[tileX][ty];

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
				Tile& tile = mData[tileX][ty];

				if (tile.Edges[Direction::Up] == EdgeType::Solid || (pHasInterest && tile.Edges[Direction::Up] == EdgeType::Interesting))
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
				Tile& tile = mData[tx][tileY];

				if (tile.Edges[Direction::Right] == EdgeType::Solid || (pHasInterest && tile.Edges[Direction::Right] == EdgeType::Interesting))
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
				Tile& tile = mData[tx][tileY];

				if (tile.Edges[Direction::Left] == EdgeType::Solid || (pHasInterest && tile.Edges[Direction::Left] == EdgeType::Interesting))
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

	Tile& tile = mData[pTileX][pTileY];

	return tile.CollisionID < 0;
}

bool Map::IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY)
{
	if (pTileX < 0 || pTileY < 0 || pTileX >= mWidth || pTileY >= mHeight)
		return true;

	if (pPixelX < 0 || pPixelY < 0 || pPixelX >= mTileset.Size || pPixelY >= mTileset.Size)
		return true;

	Tile& tile = mData[pTileX][pTileY];

	if (tile.CollisionID < 0)
		return true;

	return !mBitMasks[tile.CollisionID].GetBit(pPixelX, pPixelY);
}

void Map::Render(const Renderer& pRenderer)
{
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (!IsTileEmpty(x, y))
			{
				Tile& tile = mData[x][y];

				HGF::Vector2 position(x * mTileset.Size, y * mTileset.Size);
				HGF::Vector2 dimensions(mTileset.Size, mTileset.Size);
				HGF::Vector2 min((tile.TextureID % mTileset.X) * mTileset.Size, (tile.TextureID / mTileset.X)  * mTileset.Size);
				HGF::Vector2 max(((tile.TextureID % mTileset.X) + 1)  * mTileset.Size, ((tile.TextureID / mTileset.X) + 1)  * mTileset.Size);

				pRenderer.RenderTexture(mTileset.Texture, position, dimensions, HGF::Vector2::Zero, min, max);
			}
		}
	}
}

void Map::RenderDebug(const Renderer& pRenderer)
{
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (!IsTileEmpty(x, y))
			{
				Tile& tile = mData[x][y];

				for (int i = 0; i < 4; ++i)
				{
					if (tile.Edges[i] == EdgeType::Solid || tile.Edges[i] == EdgeType::Interesting)
					{
						float r = tile.Edges[i] == EdgeType::Interesting ? 0.0f : 0.8f;
						float g = 0.0f;
						float b = tile.Edges[i] == EdgeType::Interesting ? 0.8f : 0.0f;

						switch (i)
						{
						case Direction::Up:
							pRenderer.RenderLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), 2.0f, r, g, b, 1.0f);
							break;
						case Direction::Down:
							pRenderer.RenderLine(HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), 2.0f, r, g, b, 1.0f);
							break;
						case Direction::Left:
							pRenderer.RenderLine(HGF::Vector2(x * mTileset.Size, y * mTileset.Size), HGF::Vector2(x * mTileset.Size, (y + 1) * mTileset.Size), 2.0f, r, g, b, 1.0f);
							break;
						case Direction::Right:
							pRenderer.RenderLine(HGF::Vector2((x + 1) * mTileset.Size, y * mTileset.Size), HGF::Vector2((x + 1) * mTileset.Size, (y + 1) * mTileset.Size), 2.0f, r, g, b, 1.0f);
							break;
						}
					}
				}
			}
		}
	}
}

void Map::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (!IsTileEmpty(x, y))
			{
				Tile& tile = mData[x][y];

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

void Map::RenderDebug(SAGE::GeometryBatch& pGeometryBatch)
{
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			if (!IsTileEmpty(x, y))
			{
				Tile& tile = mData[x][y];

				for (int i = 0; i < 4; ++i)
				{
					if (tile.Edges[i] == EdgeType::Solid || tile.Edges[i] == EdgeType::Interesting)
					{
						float r = tile.Edges[i] == EdgeType::Interesting ? 0.0f : 0.8f;
						float g = 0.0f;
						float b = tile.Edges[i] == EdgeType::Interesting ? 0.8f : 0.0f;

						HGF::Color color(r, g, b);

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