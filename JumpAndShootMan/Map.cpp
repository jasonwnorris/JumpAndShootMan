#include "Map.hpp"

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include <cstdlib>

Uint32 GetPixel(SDL_Surface* pSurface, int pX, int pY)
{
	Uint8* pixel = (Uint8*)pSurface->pixels + pY * pSurface->pitch + pX * pSurface->format->BytesPerPixel;

	switch (pSurface->format->BytesPerPixel)
	{
	case 1:
		return *pixel;
	case 2:
		return *(Uint16*)pixel;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
		else
			return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
	case 4:
		return *(Uint32*)pixel;
	default:
		return 0;
	}
}

Map::Map()
{
	Tileset.X = 3;
	Tileset.Y = 3;
	Tileset.Size = 16;
	Width = 16;
	Height = 12;

	SDL_Surface* surface = IMG_Load("data/img/bitfield.png");
	if (surface != NULL)
	{
		// Establish bitmask count.
		BitMaskCount = surface->w / Tileset.Size;
		BitMasks = new bool**[BitMaskCount];

		for (int i = 0; i < BitMaskCount; ++i)
		{
			BitMasks[i] = new bool*[Tileset.Size];

			for (int px = 0; px < Tileset.Size; ++px)
			{
				BitMasks[i][px] = new bool[Tileset.Size];

				for (int py = 0; py < Tileset.Size; ++py)
				{
					Uint32 color = GetPixel(surface, i * Tileset.Size + px, 1 * Tileset.Size + py);
					Uint8 r, g, b, a;
					SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);

					BitMasks[i][px][py] = (r == 0 && g == 0 && b == 0);
				}
			}
		}

		SDL_FreeSurface(surface);
	}

	Data = new Tile*[Width];

	for (int x = 0; x < Width; ++x)
	{
		Data[x] = new Tile[Height];

		for (int y = 0; y < Height; ++y)
		{
			Tile* tile = &Data[x][y];

			if (x == 0 || x == Width - 1 || y == 0 || y == Height - 1)
			{
				tile->TextureID = 0;
				tile->CollisionID = 0;
			}
			else if (rand() % 12 == 0)
			{
				tile->TextureID = rand() % (Tileset.X * Tileset.Y - 1) + 1;

				if (tile->TextureID == 7)
				{
					tile->CollisionID = 1;
				}
				else if (tile->TextureID == 8)
				{
					tile->CollisionID = 2;
				}
				else
				{
					tile->CollisionID = 0;
				}
			}
			else
			{
				tile->TextureID = -1;
				tile->CollisionID = -1;
			}
		}
	}
}

Map::~Map()
{
	// Free map data.
	for (int x = 0; x < Width; ++x)
	{
		delete[] Data[x];
	}
	delete[] Data;

	// Free map bitmasks.
	for (int i = 0; i < BitMaskCount; ++i)
	{
		for (int x = 0; x < Tileset.Size; ++x)
		{
			delete[] BitMasks[i][x];
		}
		delete[] BitMasks[i];
	}
	delete[] BitMasks;
}