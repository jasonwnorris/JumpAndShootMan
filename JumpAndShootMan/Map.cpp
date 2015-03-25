#include "Globals.hpp"
#include "Map.hpp"
#include "Renderer.hpp"

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include <cstdlib>
#include <iostream>

template<typename T> bool VECTORHELPER_Contains(const std::vector<T>& pVector, T pItem)
{
	return std::find(pVector.begin(), pVector.end(), pItem) != pVector.end();
}

Map::Map()
{
	mIsLoaded = false;
}

Map::~Map()
{
	// Free map data.
	for (int x = 0; x < Width; ++x)
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

	// Temp default values.
	if (!Tileset.Texture.Load("data/img/tiles.png"))
		return false;

	Tileset.X = 2;
	Tileset.Y = 2;
	Tileset.Size = 16;
	Width = 16;
	Height = 12;

	// Load bitmask tiles.
	HGF::Texture bitmaskTexture;
	if (!bitmaskTexture.Load("data/img/bitmasks.png"))
		return false;

	// Establish bitmask count.
	BitMaskCount = bitmaskTexture.GetWidth() / Tileset.Size;
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

	// Debug: Print the bitmasks.
	for (int i = 0; i < BitMaskCount; ++i)
	{
		std::cout << "Bitmask #" << i << ":" << std::endl;

		for (int px = 0; px < Tileset.Size; ++px)
		{
			for (int py = 0; py < Tileset.Size; ++py)
			{
				std::cout << (BitMasks[i][px][py] ? "1" : "0");
			}

			std::cout << std::endl;
		}

		std::cout << std::endl;
	}

	// Allocate tile memory.
	Data = new Tile*[Width];
	for (int x = 0; x < Width; ++x)
	{
		Data[x] = new Tile[Height];
	}

	mIsLoaded = true;

	Randomize();

	return mIsLoaded;
}

bool Map::Randomize()
{
	if (!mIsLoaded)
		return false;

	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Height; ++y)
		{
			Tile* tile = &Data[x][y];

			if (x == 0 || x == Width - 1 || y == 0 || y == Height - 1)
			{
				tile->TextureID = 0;
				tile->CollisionID = 0;
				for (int i = 0; i < 4; ++i)
					tile->Edges[i] = EdgeType::Solid;
			}
			else if (rand() % 12 == 0)
			{
				tile->TextureID = rand() % (Tileset.X * Tileset.Y - 1) + 1;

				if (tile->TextureID == 2)
				{
					tile->CollisionID = 1;
					tile->Edges[Direction::Up] = EdgeType::Interesting;
					tile->Edges[Direction::Down] = EdgeType::Solid;
					tile->Edges[Direction::Left] = EdgeType::Interesting;
					tile->Edges[Direction::Right] = EdgeType::Solid;
				}
				else if (tile->TextureID == 3)
				{
					tile->CollisionID = 2;
					tile->Edges[Direction::Up] = EdgeType::Interesting;
					tile->Edges[Direction::Down] = EdgeType::Solid;
					tile->Edges[Direction::Left] = EdgeType::Solid;
					tile->Edges[Direction::Right] = EdgeType::Interesting;
				}
				else
				{
					tile->CollisionID = 0;
					for (int i = 0; i < 4; ++i)
						tile->Edges[i] = EdgeType::Solid;
				}
			}
			else
			{
				tile->TextureID = -1;
				tile->CollisionID = -1;
			}
		}
	}

	return true;
}

bool Map::Raycast(const HGF::Vector2& pPosition, Direction pDirection, const std::vector<int>& pTargets, HGF::Vector2& pOutHit, float& pOutDistance)
{
	int x = (int)std::roundf(pPosition.X);
	int y = (int)std::roundf(pPosition.Y);
	int tileX = x / Tileset.Size;
	int tileY = y / Tileset.Size;
	int pixelX = x % Tileset.Size;
	int pixelY = y % Tileset.Size;

	pOutHit = HGF::Vector2::Zero;

	switch (pDirection)
	{
		case Direction::Up:
			for (int ty = tileY; ty >= 0; --ty)
			{
				if (!IsTileEmpty(tileX, ty))
				{
					for (int py = Tileset.Size - 1; py >= 0; --py)
					{
						if (!IsTraversable(tileX, ty, pixelX, py) && VECTORHELPER_Contains(pTargets, Data[tileX][ty].CollisionID))
						{
							pOutHit = HGF::Vector2(pPosition.X, (float)(ty * Tileset.Size + py));
							pOutDistance = pPosition.Y - pOutHit.Y;
							return true;
						}
					}
				}
			}
			break;
		case Direction::Down:
			for (int ty = tileY; ty < Height; ++ty)
			{
				if (!IsTileEmpty(tileX, ty))
				{
					for (int py = 0; py < Tileset.Size; ++py)
					{
						if (!IsTraversable(tileX, ty, pixelX, py) && VECTORHELPER_Contains(pTargets, Data[tileX][ty].CollisionID))
						{
							pOutHit = HGF::Vector2(pPosition.X, (float)(ty * Tileset.Size + py));
							pOutDistance = pOutHit.Y - pPosition.Y;
							return true;
						}
					}
				}
			}
			break;
		case Direction::Left:
			for (int tx = tileX; tx >= 0; --tx)
			{
				if (!IsTileEmpty(tx, tileY))
				{
					for (int px = Tileset.Size - 1; px >= 0; --px)
					{
						if (!IsTraversable(tx, tileY, px, pixelY) && VECTORHELPER_Contains(pTargets, Data[tx][tileY].CollisionID))
						{
							pOutHit = HGF::Vector2((float)(tx * Tileset.Size + px), pPosition.Y);
							pOutDistance = pPosition.X - pOutHit.X;
							return true;
						}
					}
				}
			}
			break;
		case Direction::Right:
			for (int tx = tileX; tx < Width; ++tx)
			{
				if (!IsTileEmpty(tx, tileY))
				{
					for (int px = 0; px < Tileset.Size; ++px)
					{
						if (!IsTraversable(tx, tileY, px, pixelY) && VECTORHELPER_Contains(pTargets, Data[tx][tileY].CollisionID))
						{
							pOutHit = HGF::Vector2((float)(tx * Tileset.Size + px), pPosition.Y);
							pOutDistance = pOutHit.X - pPosition.X;
							return true;
						}
					}
				}
			}
			break;
	}

	return false;
}

bool Map::IsTileEmpty(int pTileX, int pTileY)
{
	if (pTileX < 0 || pTileY < 0 || pTileX >= Width || pTileY >= Height)
		return true;

	return Data[pTileX][pTileY].CollisionID < 0;
}

bool Map::IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY)
{
	if (pTileX < 0 || pTileY < 0 || pTileX >= Width || pTileY >= Height)
		return true;

	if (pPixelX < 0 || pPixelY < 0 || pPixelX >= Tileset.Size || pPixelY >= Tileset.Size)
		return true;

	if (Data[pTileX][pTileY].CollisionID < 0)
		return true;

	return !BitMasks[Data[pTileX][pTileY].CollisionID][pPixelX][pPixelY];
}

bool Map::Render(const Renderer& pRenderer)
{
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int value = Data[x][y].TextureID;
			if (value >= 0)
			{
				HGF::Vector2 position(x * Tileset.Size, y * Tileset.Size);
				HGF::Vector2 dimensions(Tileset.Size, Tileset.Size);
				HGF::Vector2 min((value % Tileset.Y) * Tileset.Size, (value / Tileset.Y)  * Tileset.Size);
				HGF::Vector2 max(((value % Tileset.Y) + 1)  * Tileset.Size, ((value / Tileset.Y) + 1)  * Tileset.Size);

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