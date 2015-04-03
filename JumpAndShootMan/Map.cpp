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
		if (Data[x])
		{
			delete [] Data[x];
		}
	}
	if (Data)
	{
		delete [] Data;
	}

	// Free map bitmasks.
	for (int i = 0; i < BitMaskCount; ++i)
	{
		for (int x = 0; x < Tileset.Size; ++x)
		{
			if (BitMasks[i][x])
			{
				delete [] BitMasks[i][x];
			}
		}
		if (BitMasks[i])
		{
			delete [] BitMasks[i];
		}
	}
	if (BitMasks)
	{
		delete [] BitMasks;
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
	file >> Tileset.X >> Tileset.Y >> Tileset.Size;
	file >> bitmaskFilename;
	file >> bitWidth >> bitHeight >> bitSize;

	// Load tileset texture;
	if (!Tileset.Texture.Load(tilesetFilename))
		return false;

	// Load bitmask texture.
	HGF::Texture bitmaskTexture;
	if (!bitmaskTexture.Load(bitmaskFilename))
		return false;

	// Establish bitmask count.
	BitMaskCount = bitWidth * bitHeight;
	BitMasks = new bool**[BitMaskCount];

	// Read the bitmask pixels.
	for (int i = 0; i < BitMaskCount; ++i)
	{
		BitMasks[i] = new bool*[Tileset.Size];

		for (int px = 0; px < Tileset.Size; ++px)
		{
			BitMasks[i][px] = new bool[Tileset.Size];

			for (int py = 0; py < Tileset.Size; ++py)
			{
				Uint8 r, g, b, a;
				bitmaskTexture.GetColor(i * Tileset.Size + px, py, r, g, b, a);

				BitMasks[i][px][py] = (r == 0 && g == 0 && b == 0);
			}
		}
	}

	// Allocate tile memory.
	Data = new Tile*[mWidth];
	for (int x = 0; x < mWidth; ++x)
	{
		Data[x] = new Tile[mHeight];
	}

	// Load tile values.
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			Tile& tile = Data[x][y];

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
				if (Data[x][y].Edges[Direction::Right] == EdgeType::Solid && Data[x + 1][y].Edges[Direction::Left] == EdgeType::Solid)
				{
					Data[x][y].Edges[Direction::Right] = EdgeType::Empty;
					Data[x + 1][y].Edges[Direction::Left] = EdgeType::Empty;
				}
			}
			if (y + 1 != mHeight)
			{
				if (Data[x][y].Edges[Direction::Down] == EdgeType::Solid && Data[x][y + 1].Edges[Direction::Up] == EdgeType::Solid)
				{
					Data[x][y].Edges[Direction::Down] = EdgeType::Empty;
					Data[x][y + 1].Edges[Direction::Up] = EdgeType::Empty;
				}
			}
		}
	}

	file.close();

	mIsLoaded = true;

	return mIsLoaded;
}

bool Map::Randomize()
{
	if (!mIsLoaded)
		return false;

	// Generate tiles.
	for (int x = 0; x < mWidth; ++x)
	{
		for (int y = 0; y < mHeight; ++y)
		{
			Tile& tile = Data[x][y];

			if (x == 0 || x == mWidth - 1 || y == 0 || y == mHeight - 1)
			{
				tile.TextureID = 0;
				tile.CollisionID = 0;
				for (int i = 0; i < 4; ++i)
					tile.Edges[i] = EdgeType::Solid;
			}
			else if (rand() % 15 == 0)
			{
				tile.TextureID = rand() % (Tileset.X * Tileset.Y);

				if (tile.TextureID == 2)
				{
					tile.CollisionID = 1;
					tile.Edges[Direction::Up] = EdgeType::Interesting;
					tile.Edges[Direction::Down] = EdgeType::Solid;
					tile.Edges[Direction::Left] = EdgeType::Interesting;
					tile.Edges[Direction::Right] = EdgeType::Solid;
				}
				else if (tile.TextureID == 3)
				{
					tile.CollisionID = 2;
					tile.Edges[Direction::Up] = EdgeType::Interesting;
					tile.Edges[Direction::Down] = EdgeType::Solid;
					tile.Edges[Direction::Left] = EdgeType::Solid;
					tile.Edges[Direction::Right] = EdgeType::Interesting;
				}
				else
				{
					tile.CollisionID = 0;
					for (int i = 0; i < 4; ++i)
						tile.Edges[i] = EdgeType::Solid;
				}
			}
			else
			{
				tile.TextureID = -1;
				tile.CollisionID = -1;
				for (int i = 0; i < 4; ++i)
					tile.Edges[i] = EdgeType::Empty;
			}
		}
	}

	// Resolve collision edges.
	for (int x = 0; x < mWidth; ++x)
	{
		for (int y = 0; y < mHeight; ++y)
		{
			if (x + 1 != mWidth)
			{
				if (Data[x][y].Edges[Direction::Right] == EdgeType::Solid && Data[x + 1][y].Edges[Direction::Left] == EdgeType::Solid)
				{
					Data[x][y].Edges[Direction::Right] = EdgeType::Empty;
					Data[x + 1][y].Edges[Direction::Left] = EdgeType::Empty;
				}
			}
			if (y + 1 != mHeight)
			{
				if (Data[x][y].Edges[Direction::Down] == EdgeType::Solid && Data[x][y + 1].Edges[Direction::Up] == EdgeType::Solid)
				{
					Data[x][y].Edges[Direction::Down] = EdgeType::Empty;
					Data[x][y + 1].Edges[Direction::Up] = EdgeType::Empty;
				}
			}
		}
	}

	return true;
}

void Map::Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit)
{
	int x = (int)std::roundf(pPosition.X);
	int y = (int)std::roundf(pPosition.Y);
	int tileX = x / Tileset.Size;
	int tileY = y / Tileset.Size;
	int pixelX = x % Tileset.Size;
	int pixelY = y % Tileset.Size;

	float maxDistance = 1000.0f;
	float left = 0.0f;
	float top = 0.0f;
	float right = (float)(mWidth * Tileset.Size);
	float bottom = (float)(mHeight * Tileset.Size);

	pRaycastHit.Position = HGF::Vector2::Zero;
	pRaycastHit.Distance = maxDistance;

	switch (pDirection)
	{
		case Direction::Up:
			for (int ty = tileY - 1; ty >= 0; --ty)
			{
				if (Data[tileX][ty].Edges[Direction::Down] == EdgeType::Solid || (pHasInterest && Data[tileX][ty].Edges[Direction::Down] == EdgeType::Interesting))
				{
					for (int py = Tileset.Size - 1; py >= 0; --py)
					{
						if (!IsTraversable(tileX, ty, pixelX, py))
						{
							pRaycastHit.Position = HGF::Vector2(pPosition.X, (float)(ty * Tileset.Size + py));
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
			for (int ty = tileY + 1; ty < mHeight; ++ty)
			{
				if (Data[tileX][ty].Edges[Direction::Up] == EdgeType::Solid || (pHasInterest && Data[tileX][ty].Edges[Direction::Up] == EdgeType::Interesting))
				{
					for (int py = 0; py < Tileset.Size; ++py)
					{
						if (!IsTraversable(tileX, ty, pixelX, py))
						{
							pRaycastHit.Position = HGF::Vector2(pPosition.X, (float)(ty * Tileset.Size + py));
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
			for (int tx = tileX - 1; tx >= 0; --tx)
			{
				if (Data[tx][tileY].Edges[Direction::Right] == EdgeType::Solid || (pHasInterest && Data[tx][tileY].Edges[Direction::Right] == EdgeType::Interesting))
				{
					for (int px = Tileset.Size - 1; px >= 0; --px)
					{
						if (!IsTraversable(tx, tileY, px, pixelY))
						{
							pRaycastHit.Position = HGF::Vector2((float)(tx * Tileset.Size + px), pPosition.Y);
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
			for (int tx = tileX + 1; tx < mWidth; ++tx)
			{
				if (Data[tx][tileY].Edges[Direction::Left] == EdgeType::Solid || (pHasInterest && Data[tx][tileY].Edges[Direction::Left] == EdgeType::Interesting))
				{
					for (int px = 0; px < Tileset.Size; ++px)
					{
						if (!IsTraversable(tx, tileY, px, pixelY))
						{
							pRaycastHit.Position = HGF::Vector2((float)(tx * Tileset.Size + px), pPosition.Y);
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

	return Data[pTileX][pTileY].CollisionID < 0;
}

bool Map::IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY)
{
	if (pTileX < 0 || pTileY < 0 || pTileX >= mWidth || pTileY >= mHeight)
		return true;

	if (pPixelX < 0 || pPixelY < 0 || pPixelX >= Tileset.Size || pPixelY >= Tileset.Size)
		return true;

	if (Data[pTileX][pTileY].CollisionID < 0)
		return true;

	return !BitMasks[Data[pTileX][pTileY].CollisionID][pPixelX][pPixelY];
}

bool Map::Render(const Renderer& pRenderer)
{
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			int value = Data[x][y].TextureID;
			if (value >= 0)
			{
				HGF::Vector2 position(x * Tileset.Size, y * Tileset.Size);
				HGF::Vector2 dimensions(Tileset.Size, Tileset.Size);
				HGF::Vector2 min((value % Tileset.X) * Tileset.Size, (value / Tileset.X)  * Tileset.Size);
				HGF::Vector2 max(((value % Tileset.X) + 1)  * Tileset.Size, ((value / Tileset.X) + 1)  * Tileset.Size);

				if (pRenderer.RenderTexture(Tileset.Texture, position, dimensions, HGF::Vector2::Zero, min, max) < 0)
					return false;

				if (Globals::IsDebugDrawOn)
				{
					for (int i = 0; i < 4; ++i)
					{
						if (Data[x][y].Edges[i] == EdgeType::Solid || Data[x][y].Edges[i] == EdgeType::Interesting)
						{
							float r = Data[x][y].Edges[i] == EdgeType::Interesting ? 0.0f : 0.8f;
							float g = 0.0f;
							float b = Data[x][y].Edges[i] == EdgeType::Interesting ? 0.8f : 0.0f;

							switch (i)
							{
							case Direction::Up:
								if (pRenderer.RenderLine(HGF::Vector2(x * Tileset.Size, y * Tileset.Size), HGF::Vector2((x + 1) * Tileset.Size, y * Tileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							case Direction::Down:
								if (pRenderer.RenderLine(HGF::Vector2(x * Tileset.Size, (y + 1) * Tileset.Size), HGF::Vector2((x + 1) * Tileset.Size, (y + 1) * Tileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							case Direction::Left:
								if (pRenderer.RenderLine(HGF::Vector2(x * Tileset.Size, y * Tileset.Size), HGF::Vector2(x * Tileset.Size, (y + 1) * Tileset.Size), 2.0f, r, g, b, 1.0f))
									return false;
								break;
							case Direction::Right:
								if (pRenderer.RenderLine(HGF::Vector2((x + 1) * Tileset.Size, y * Tileset.Size), HGF::Vector2((x + 1) * Tileset.Size, (y + 1) * Tileset.Size), 2.0f, r, g, b, 1.0f))
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