#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include "BitArray.hpp"
#include "Vector2.hpp"

#include <iostream>
#include <map>
#include <time.h>
#include <vector>

const float EPSILON = 0.0001f;
const int MAX_MARKER_COUNT = 11;

enum MarkerIndex
{
	MRK_HOTSPOT,
	MRK_FOOT_LEFT,
	MRK_FOOT_RIGHT,
	MRK_LEFT_CENTER,
	MRK_LEFT_TOP,
	MRK_LEFT_BOTTOM,
	MRK_RIGHT_CENTER,
	MRK_RIGHT_TOP,
	MRK_RIGHT_BOTTOM,
	MRK_HEAD_LEFT,
	MRK_HEAD_RIGHT
};

struct CollisionMarker
{
	HGF::Vector2 Position;
	bool IsTouching;
};

struct Player
{
	SDL_Texture* Texture;
	HGF::Vector2 Position;
	HGF::Vector2 Velocity;
	HGF::Vector2 Acceleration;
	bool IsFacingLeft;
	float MovementSpeed;
	float JumpingSpeed;
	float Gravity;
	HGF::Vector2 Dimensions;
	bool IsGrounded;
	bool IsJumping;
	CollisionMarker CollisionMarkers[MAX_MARKER_COUNT];
};

struct Camera
{
	HGF::Vector2 Position;
	float Rotation;
	float Zoom;
};

struct Tileset
{
	SDL_Texture* Texture;
	int X;
	int Y;
	int Size;
};

struct Tile
{
	int TextureID;
	int CollisionID;
};

struct Map
{
	Tileset Tileset;
	int Width;
	int Height;
	Tile* Data;
	std::vector<std::vector<bool>> BitMasks;
};

struct HitData
{
	int TileX;
	int TileY;
	int PixelX;
	int PixelY;
};

int Abs(int pValue)
{
	return (pValue < 0) ? -pValue : pValue;
}

int Min(int pValueA, int pValueB)
{
	return (pValueA < pValueB) ? pValueA : pValueB;
}

int Max(int pValueA, int pValueB)
{
	return (pValueA > pValueB) ? pValueA : pValueB;
}

float Abs(float pValue)
{
	return (pValue < 0.0f) ? -pValue : pValue;
}

float Min(float pValueA, float pValueB)
{
	return (pValueA < pValueB) ? pValueA : pValueB;
}

float Max(float pValueA, float pValueB)
{
	return (pValueA > pValueB) ? pValueA : pValueB;
}

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

void GenerateMap(Map& pMap)
{
	SDL_Surface* surface = IMG_Load("data/img/bitfield.png");
	if (surface != NULL)
	{
		int cx = surface->w / pMap.Tileset.Size;
		int cy = surface->h / pMap.Tileset.Size;

		for (int ty = 0; ty < cy; ++ty)
		{
			for (int tx = 0; tx < cx; ++tx)
			{
				std::vector<bool> bits;

				for (int py = 0; py < pMap.Tileset.Size; ++py)
				{
					for (int px = 0; px < pMap.Tileset.Size; ++px)
					{
						Uint32 color = GetPixel(surface, tx * pMap.Tileset.Size + px, ty * pMap.Tileset.Size + py);
						Uint8 r, g, b, a;
						SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);
						bits.push_back(r == 0 && g == 0 && b == 0);
					}
				}

				pMap.BitMasks.push_back(bits);
			}
		}

		SDL_FreeSurface(surface);
	}

	for (int y = 0; y < pMap.Height; ++y)
	{
		for (int x = 0; x < pMap.Width; ++x)
		{
			Tile* tile = &pMap.Data[x * pMap.Height + y];

			if (x == 0 || x == pMap.Width - 1 || y == 0 || y == pMap.Height - 1)
			{
				tile->TextureID = 0;
				tile->CollisionID = 0;
			}
			else if (rand() % 12 == 0)
			{
				tile->TextureID = rand() % (pMap.Tileset.X * pMap.Tileset.Y - 1) + 1;

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

SDL_Texture* LoadTexture(SDL_Renderer* pRenderer, const char* pFilename)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load(pFilename);
	if (surface != NULL)
	{
		texture = SDL_CreateTextureFromSurface(pRenderer, surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

int RenderPoint(SDL_Renderer* pRenderer, const Camera& pCamera, HGF::Vector2 pPosition, float pSize = 1.0f, Uint8 pRed = 255, Uint8 pGreen = 255, Uint8 pBlue = 255, Uint8 pAlpha = 255)
{
	SDL_Rect rect = {
		(int)((pPosition.X + pCamera.Position.X - pSize / 2.0f) * pCamera.Zoom),
		(int)((pPosition.Y + pCamera.Position.Y - pSize / 2.0f) * pCamera.Zoom),
		(int)(pSize * pCamera.Zoom),
		(int)(pSize * pCamera.Zoom)
	};

	SDL_SetRenderDrawColor(pRenderer, pRed, pGreen, pBlue, pAlpha);
	if (SDL_RenderFillRect(pRenderer, &rect) < 0)
		return -1;

	return 0;
}

int RenderRectangle(SDL_Renderer* pRenderer, const Camera& pCamera, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, Uint8 pRed = 255, Uint8 pGreen = 255, Uint8 pBlue = 255, Uint8 pAlpha = 255)
{
	SDL_Point points[5] = {
		{ (int)((pPosition.X + pCamera.Position.X) * pCamera.Zoom),					(int)((pPosition.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom), (int)((pPosition.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom), (int)((pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)((pPosition.X + pCamera.Position.X) * pCamera.Zoom),					(int)((pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)((pPosition.X + pCamera.Position.X) * pCamera.Zoom),					(int)((pPosition.Y + pCamera.Position.Y) * pCamera.Zoom) }
	};

	SDL_SetRenderDrawColor(pRenderer, pRed, pGreen, pBlue, pAlpha);
	if (SDL_RenderDrawLines(pRenderer, points, 5) < 0)
		return -1;

	return 0;
}

int RenderTexture(SDL_Renderer* pRenderer, const Camera& pCamera, SDL_Texture* pTexture, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pClipLocation, HGF::Vector2 pClipSize, bool pFlip = false)
{
	SDL_Rect source = {
		(int)(pClipLocation.X),
		(int)(pClipLocation.Y),
		(int)(pClipSize.X),
		(int)(pClipSize.Y)
	};
	SDL_Rect destination = {
		(int)((pPosition.X + pCamera.Position.X) * pCamera.Zoom),
		(int)((pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		(int)((pDimensions.X) * pCamera.Zoom),
		(int)((pDimensions.Y) * pCamera.Zoom)
	};

	return SDL_RenderCopyEx(pRenderer, pTexture, &source, &destination, 0.0, NULL, pFlip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

int RenderPlayer(SDL_Renderer* pRenderer, const Camera& pCamera, const Player& pPlayer)
{
	// texture
	if (RenderTexture(pRenderer, pCamera, pPlayer.Texture, pPlayer.Position, pPlayer.Dimensions, HGF::Vector2::Zero, HGF::Vector2(140.0f, 184.0f), pPlayer.IsFacingLeft) < 0)
		return -1;

	// AABB
	if (RenderRectangle(pRenderer, pCamera, pPlayer.Position, pPlayer.Dimensions, 0, 255, 0) < 0)
		return -1;

	// collision points
	for (int i = 0; i < MAX_MARKER_COUNT; ++i)
	{
		int r = pPlayer.CollisionMarkers[i].IsTouching ? 150 : 50;
		int g = pPlayer.CollisionMarkers[i].IsTouching ? 175 : 75;
		int b = pPlayer.CollisionMarkers[i].IsTouching ? 180 : 80;
		if (RenderPoint(pRenderer, pCamera, pPlayer.Position + pPlayer.CollisionMarkers[i].Position, 2.0f, r, g, b) < 0)
			return -1;
	}

	return 0;
}

int RenderMap(SDL_Renderer* pRenderer, const Camera& pCamera, const Map& pMap)
{
	for (int y = 0; y < pMap.Height; ++y)
	{
		for (int x = 0; x < pMap.Width; ++x)
		{
			int value = pMap.Data[x * pMap.Height + y].TextureID;
			if (value >= 0)
			{
				if (RenderTexture(pRenderer,
								  pCamera,
								  pMap.Tileset.Texture,
								  HGF::Vector2(x * pMap.Tileset.Size, y * pMap.Tileset.Size),
								  HGF::Vector2(pMap.Tileset.Size, pMap.Tileset.Size),
								  HGF::Vector2((value % pMap.Tileset.Y) * pMap.Tileset.Size, (value / pMap.Tileset.Y) * pMap.Tileset.Size),
								  HGF::Vector2(pMap.Tileset.Size, pMap.Tileset.Size)) < 0)
					return -1;
			}
		}
	}

	return 0;
}

bool CheckPoint(const Map& pMap, const HGF::Vector2& pPosition, HitData& pHitData)
{
	pHitData.TileX = (int)(pPosition.X) / pMap.Tileset.Size;
	pHitData.TileY = (int)(pPosition.Y) / pMap.Tileset.Size;
	pHitData.PixelX = (int)(pPosition.X) % pMap.Tileset.Size;
	pHitData.PixelY = (int)(pPosition.Y) % pMap.Tileset.Size;

	if (pHitData.TileX < 0 || pHitData.TileX >= pMap.Width || pHitData.TileY < 0 || pHitData.TileY >= pMap.Height)
		return false;

	Tile* tile = &pMap.Data[pHitData.TileX * pMap.Height + pHitData.TileY];

	if (tile->CollisionID < 0)
		return false;

	return pMap.BitMasks[tile->CollisionID][pHitData.PixelX * pMap.Tileset.Size + pHitData.PixelY];
}

bool IsBitMaskHit(const Map& pMap, int pX, int pY, int& pCollisionID)
{
	int tileX = pX / pMap.Tileset.Size;
	int tileY = pY / pMap.Tileset.Size;
	int pixelX = pX % pMap.Tileset.Size;
	int pixelY = pY % pMap.Tileset.Size;

	if (tileX < 0 || tileX >= pMap.Width || tileY < 0 || tileY >= pMap.Height)
		return false;

	Tile* tile = &pMap.Data[tileX * pMap.Height + tileY];
	if (tile->CollisionID < 0)
		return false;

	pCollisionID = tile->CollisionID;

	return pMap.BitMasks[pCollisionID][pixelX + pixelY * pMap.Tileset.Size];
}

void HandleWorldCollisionViaPoints(Player& pPlayer, const Map& pMap)
{
	int id = -1;

	// left/right collision
	bool hitHorz = false;
	if (pPlayer.Velocity.X < -EPSILON)
	{
		// left collision
		for (int i = 3; i <= 5; ++i)
		{
			// 3 = MRK_LEFT_CENTER
			// 4 = MRK_LEFT_TOP
			// 5 = MRK_LEFT_BOTTOM
			int initial = (int)pPlayer.Position.X;
			int start = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
			int finish = (int)(pPlayer.Position.X + pPlayer.Velocity.X + pPlayer.CollisionMarkers[i].Position.X);
			int y = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
			for (int x = start; x >= finish; --x)
			{
				if (IsBitMaskHit(pMap, x, y, id))
				{
					if (id == 0)
					{
						pPlayer.Position.X = (float)(x - (initial - start));
						pPlayer.Velocity.X = 0.0f;
						hitHorz = true;
						break;
					}
				}
			}
			if (hitHorz)
				break;
		}
	}
	else if (pPlayer.Velocity.X > EPSILON)
	{
		// right collision
		for (int i = 6; i <= 8; ++i)
		{
			bool hit = false;
			// 6 = MRK_RIGHT_CENTER
			// 7 = MRK_RIGHT_TOP
			// 8 = MRK_RIGHT_BOTTOM
			int initial = (int)pPlayer.Position.X;
			int start = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
			int finish = (int)(pPlayer.Position.X + pPlayer.Velocity.X + pPlayer.CollisionMarkers[i].Position.X);
			int y = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
			for (int x = start; x <= finish; ++x)
			{
				if (IsBitMaskHit(pMap, x, y, id))
				{
					if (id == 0)
					{
						pPlayer.Position.X = (float)(x - (start - initial));
						pPlayer.Velocity.X = 0.0f;
						hitHorz = true;
						break;
					}
				}
			}
			if (hitHorz)
				break;
		}
	}

	if (!hitHorz)
		pPlayer.Position.X += pPlayer.Velocity.X;

	// bottom/top collision
	bool hitVert = false;
	if (pPlayer.Velocity.Y > EPSILON)
	{
		// bottom collision
		for (int i = 0; i <= 2; ++i)
		{
			// 0 = MRK_HOTSPOT
			// 1 = MRK_FOOT_LEFT
			// 2 = MRK_FOOT_RIGHT
			int initial = (int)pPlayer.Position.Y;
			int start = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
			int finish = (int)(pPlayer.Position.Y + pPlayer.Velocity.Y + pPlayer.CollisionMarkers[i].Position.Y);
			int x = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
			for (int y = start; y <= finish; ++y)
			{
				if (IsBitMaskHit(pMap, x, y, id))
				{
					if (!((i == 1 || i == 2) && id != 0))
					{
						pPlayer.Position.Y = (float)(y + (initial - start));
						pPlayer.Velocity.Y = 0.0f;
						hitVert = true;
						break;
					}
				}
			}
			if (hitVert)
				break;
		}
		pPlayer.IsGrounded = hitVert;
	}
	else if (pPlayer.Velocity.Y < -EPSILON)
	{
		// top collision
		for (int i = 9; i <= 10; ++i)
		{
			// 9 = MRK_HEAD_LEFT
			// 10 = MRK_HEAD_RIGHT
			int initial = (int)pPlayer.Position.Y;
			int start = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
			int finish = (int)(pPlayer.Position.Y + pPlayer.Velocity.Y + pPlayer.CollisionMarkers[i].Position.Y);
			int x = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
			for (int y = start; y >= finish; --y)
			{
				if (IsBitMaskHit(pMap, x, y, id))
				{
					pPlayer.Position.Y = (float)(y - (initial - start));
					pPlayer.Velocity.Y = 0.0f;
					hitVert = true;
					break;
				}
			}
			if (hitVert)
				break;
		}
	}

	if (!hitVert)
		pPlayer.Position.Y += pPlayer.Velocity.Y;

	// temp to fix tunneling
	for (int i = 0; i < MAX_MARKER_COUNT; ++i)
	{
		int x = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
		int y = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
		int original = 0;
		switch (i)
		{
			case MRK_HOTSPOT:
			case MRK_FOOT_LEFT:
			case MRK_FOOT_RIGHT:
				original = y;
				while (IsBitMaskHit(pMap, x, y, id))
				{
					if (i != MRK_HOTSPOT && id != 0)
						break;
					y--;
				}
				pPlayer.Position.Y -= (original - y);
				break;
			case MRK_LEFT_CENTER:
			case MRK_LEFT_TOP:
			case MRK_LEFT_BOTTOM:
				original = x;
				while (IsBitMaskHit(pMap, x, y, id))
				{
					if (id != 0)
						break;
					x++;
				}
				pPlayer.Position.X += (x - original);
				break;
			case MRK_RIGHT_CENTER:
			case MRK_RIGHT_TOP:
			case MRK_RIGHT_BOTTOM:
				original = x;
				while (IsBitMaskHit(pMap, x, y, id))
				{
					if (id != 0)
						break;
					x--;
				}
				pPlayer.Position.X -= (original - x);
				break;
			case MRK_HEAD_LEFT:
			case MRK_HEAD_RIGHT:
				original = y;
				while (IsBitMaskHit(pMap, x, y, id))
				{
					y++;
				}
				pPlayer.Position.Y += (y - original);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return -1;

	int windowWidth = 1024;
	int windowHeight = 768;

	SDL_Window* window = SDL_CreateWindow("JUMP AND SHOOT MAN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window == NULL)
		return -1;
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		return -1;

	SDL_Event evt;
	std::map<SDL_Keycode, bool> keys;
	bool running = true;
	int frames = 0;
	Uint32 prevTicks = 0;
	Uint32 currTicks = SDL_GetTicks();

	Map map;
	map.Tileset.Texture = LoadTexture(renderer, "data/img/tileset2.png");
	if (map.Tileset.Texture == NULL)
		return -1;
	map.Tileset.X = 3;
	map.Tileset.Y = 3;
	map.Tileset.Size = 16;
	map.Width = 16;
	map.Height = 12;
	map.Data = new Tile[map.Width * map.Height];
	GenerateMap(map);

	Player player;
	player.Texture = LoadTexture(renderer, "data/img/megamanx.png");
	if (player.Texture == NULL)
		return -1;
	player.Position = HGF::Vector2(100.0f, 100.0f);
	player.Velocity = HGF::Vector2::Zero;
	player.Acceleration = HGF::Vector2::Zero;
	player.IsFacingLeft = false;
	player.MovementSpeed = 0.185f;
	player.JumpingSpeed = 1.15f;
	player.Gravity = 0.035f;
	player.Dimensions = HGF::Vector2(30, 34);
	player.IsGrounded = false;
	player.IsJumping = false;
	for (int i = 0; i < MAX_MARKER_COUNT; ++i)
		player.CollisionMarkers[i].IsTouching = false;
	player.CollisionMarkers[MRK_HOTSPOT].Position = HGF::Vector2(player.Dimensions.X / 2.0f, player.Dimensions.Y);
	player.CollisionMarkers[MRK_FOOT_LEFT].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 8.0f, player.Dimensions.Y);
	player.CollisionMarkers[MRK_FOOT_RIGHT].Position = HGF::Vector2(player.Dimensions.X * 7.0f / 8.0f, player.Dimensions.Y);
	player.CollisionMarkers[MRK_HEAD_LEFT].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 3.0f, 0.0f);
	player.CollisionMarkers[MRK_HEAD_RIGHT].Position = HGF::Vector2(player.Dimensions.X * 2.0f / 3.0f, 0.0f);
	player.CollisionMarkers[MRK_LEFT_TOP].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 16.0f, player.Dimensions.Y * 1.0f / 6.0f);
	player.CollisionMarkers[MRK_LEFT_CENTER].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 16.0f, player.Dimensions.Y * 3.0f / 6.0f);
	player.CollisionMarkers[MRK_LEFT_BOTTOM].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 16.0f, player.Dimensions.Y * 5.0f / 6.0f);
	player.CollisionMarkers[MRK_RIGHT_TOP].Position = HGF::Vector2(player.Dimensions.X * 15.0f / 16.0f, player.Dimensions.Y * 1.0f / 6.0f);
	player.CollisionMarkers[MRK_RIGHT_CENTER].Position = HGF::Vector2(player.Dimensions.X * 15.0f / 16.0f, player.Dimensions.Y * 3.0f / 6.0f);
	player.CollisionMarkers[MRK_RIGHT_BOTTOM].Position = HGF::Vector2(player.Dimensions.X * 15.0f / 16.0f, player.Dimensions.Y * 5.0f / 6.0f);

	Camera camera;
	camera.Position = HGF::Vector2(0.0f, 0.0f);
	camera.Rotation = 0.0f;
	camera.Zoom = 2.0f;

	while (running)
	{
		// FPS
		frames++;
		currTicks = SDL_GetTicks();
		if (currTicks > prevTicks + 1000)
		{
			std::cout << "FPS: " << frames << std::endl;
			prevTicks = currTicks;
			frames = 0;
		}

		// event handling
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					keys[evt.key.keysym.sym] = true;
					break;
				case SDL_KEYUP:
					keys[evt.key.keysym.sym] = false;
					break;
				default:
					break;
			}
		}

		// debug
		if (keys[SDLK_RETURN])
		{
			GenerateMap(map);
		}
		if (keys[SDLK_BACKSPACE])
		{
			player.Position = HGF::Vector2(100.0f, 100.0f);
		}

		// handle input
		if (keys[SDLK_ESCAPE])
		{
			running = false;
		}
		if (keys[SDLK_SPACE] && player.IsGrounded && !player.CollisionMarkers[MRK_HEAD_LEFT].IsTouching && !player.CollisionMarkers[MRK_HEAD_RIGHT].IsTouching)
		{
			player.Velocity.Y = 0.0f;
			player.Acceleration.Y -= player.JumpingSpeed;
			player.IsGrounded = false;
		}
		if (keys[SDLK_LEFT])
		{
			player.Acceleration.X -= player.MovementSpeed;
			player.IsFacingLeft = true;
		}
		if (keys[SDLK_RIGHT])
		{
			player.Acceleration.X += player.MovementSpeed;
			player.IsFacingLeft = false;
		}

		// update movement
		//if (!player.IsGrounded)
		{
			player.Acceleration.Y += player.Gravity;
		}
		player.Velocity += player.Acceleration;
		player.Velocity.X *= 0.75f;
		player.Acceleration *= 0.75f;

		// handle collision
		HandleWorldCollisionViaPoints(player, map);

		// focus camera
		camera.Position.X = -player.Position.X + windowWidth / 4;
		camera.Position.Y = -player.Position.Y + windowHeight / 4;

		// clear
		SDL_SetRenderDrawColor(renderer, 150, 175, 225, 255);
		if (SDL_RenderClear(renderer) < 0)
			return -1;

		// render loop
		if (RenderMap(renderer, camera, map) < 0)
			return -1;
		if (RenderPlayer(renderer, camera, player) < 0)
			return -1;

		// flip
		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}

	// free memory
	delete [] map.Data;

	// clean up
	SDL_DestroyTexture(player.Texture);
	SDL_DestroyTexture(map.Tileset.Texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}