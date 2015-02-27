#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include "BitArray.hpp"
#include "Vector2.hpp"

#include <stdio.h>
#include <math.h>
#include <map>
#include <time.h>
#include <vector>

enum MarkerIndex
{
	MRK_HOTSPOT,
	MRK_FOOT_LEFT,
	MRK_FOOT_RIGHT,
	MRK_LEFT_TOP,
	MRK_LEFT_CENTER,
	MRK_LEFT_BOTTOM,
	MRK_RIGHT_TOP,
	MRK_RIGHT_CENTER,
	MRK_RIGHT_BOTTOM,
	MRK_HEAD_LEFT,
	MRK_HEAD_RIGHT,
	MRK_MAX_MARKER_COUNT
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
	CollisionMarker* CollisionMarkers;
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

struct Map
{
	Tileset Tileset;
	int Width;
	int Height;
	int* Data;
};

void GenerateMap(Map& pMap)
{
	for (int x = 0; x < pMap.Width; ++x)
	{
		for (int y = 0; y < pMap.Height; ++y)
		{
			int* tile = &pMap.Data[x * pMap.Height + y];

			if (x == 0 || x == pMap.Width - 1 || y == 0 || y == pMap.Height - 1)
				*tile = 0;
			else if (rand() % 12 == 0)
				*tile = rand() % (pMap.Tileset.X * pMap.Tileset.Y - 1) + 1;
			else
				*tile = -1;
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
		(int)(roundf(pPosition.X + pCamera.Position.X - pSize / 2.0f) * pCamera.Zoom),
		(int)(roundf(pPosition.Y + pCamera.Position.Y - pSize / 2.0f) * pCamera.Zoom),
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
		{ (int)(roundf(pPosition.X + pCamera.Position.X) * pCamera.Zoom),				  (int)(roundf(pPosition.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)(roundf(pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom), (int)(roundf(pPosition.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)(roundf(pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom), (int)(roundf(pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)(roundf(pPosition.X + pCamera.Position.X) * pCamera.Zoom),				  (int)(roundf(pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom) },
		{ (int)(roundf(pPosition.X + pCamera.Position.X) * pCamera.Zoom),				  (int)(roundf(pPosition.Y + pCamera.Position.Y) * pCamera.Zoom) }
	};

	SDL_SetRenderDrawColor(pRenderer, pRed, pGreen, pBlue, pAlpha);
	if (SDL_RenderDrawLines(pRenderer, points, 5) < 0)
		return -1;

	return 0;
}

int RenderTexture(SDL_Renderer* pRenderer, const Camera& pCamera, SDL_Texture* pTexture, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pClipLocation, HGF::Vector2 pClipSize, bool pFlip = false)
{
	SDL_Rect source = {
		(int)roundf(pClipLocation.X),
		(int)roundf(pClipLocation.Y),
		(int)roundf(pClipSize.X),
		(int)roundf(pClipSize.Y)
	};
	SDL_Rect destination = {
		(int)(roundf(pPosition.X + pCamera.Position.X) * pCamera.Zoom),
		(int)(roundf(pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		(int)(roundf(pDimensions.X) * pCamera.Zoom),
		(int)(roundf(pDimensions.Y) * pCamera.Zoom)
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
	for (int i = 0; i < MRK_MAX_MARKER_COUNT; ++i)
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
	for (int x = 0; x < pMap.Width; ++x)
	{
		for (int y = 0; y < pMap.Height; ++y)
		{
			int value = pMap.Data[x * pMap.Height + y];
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

bool IsColliding(const HGF::Vector2& pMinA, const HGF::Vector2& pMaxA, const HGF::Vector2& pMinB, const HGF::Vector2& pMaxB)
{
	if (pMaxA.X < pMinB.X || pMaxA.Y < pMinB.Y || pMinA.X > pMaxB.X || pMinA.Y > pMaxB.Y)
		return false;

	return true;
}

bool IsCollidingWithResponse(const HGF::Vector2& pMinA, const HGF::Vector2& pMaxA, const HGF::Vector2& pMinB, const HGF::Vector2& pMaxB, HGF::Vector2& pTranslation)
{
	float left = pMinB.X - pMaxA.X;
	float right = pMaxB.X - pMinA.X;
	float top = pMinB.Y - pMaxA.Y;
	float bottom = pMaxB.Y - pMinA.Y;

	if (left > 0.0f || right < 0.0f || top > 0.0f || bottom < 0.0f)
		return false;

	// minimum X-axis
	pTranslation.X = (fabsf(left) < fabsf(right)) ? left : right;

	// minimum Y-axis
	pTranslation.Y = (fabsf(top) < fabsf(bottom)) ? top : bottom;

	// minimal axis
	if (fabsf(pTranslation.X) < fabsf(pTranslation.Y))
		pTranslation.Y = 0.0f;
	else
		pTranslation.X = 0.0f;

	return true;
}

void HandleWorldCollisionViaAABB(Player& pPlayer, const Map& pMap)
{
	bool horizontalMovement = pPlayer.Velocity.X < 0.0f || pPlayer.Velocity.X > 0.0f;
	bool verticalMovement = pPlayer.Velocity.Y < 0.0f || pPlayer.Velocity.Y > 0.0f;

	float minX = fminf(pPlayer.Position.X, pPlayer.Position.X + pPlayer.Velocity.X);
	float maxX = fmaxf(pPlayer.Position.X + pPlayer.Dimensions.X + pPlayer.Velocity.X, pPlayer.Position.X + pPlayer.Dimensions.X);
	float minY = fminf(pPlayer.Position.Y, pPlayer.Position.Y + pPlayer.Velocity.Y);
	float maxY = fmaxf(pPlayer.Position.Y + pPlayer.Dimensions.Y + pPlayer.Velocity.Y, pPlayer.Position.Y + pPlayer.Dimensions.Y);

	int startX = (int)minX / pMap.Tileset.Size;
	int endX = (int)maxX / pMap.Tileset.Size;
	int startY = (int)minY / pMap.Tileset.Size;
	int endY = (int)maxY / pMap.Tileset.Size;

	pPlayer.Position.X += pPlayer.Velocity.X;

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			if (pMap.Data[x * pMap.Height + y] >= 0)
			{
				HGF::Vector2 tileMin = HGF::Vector2(x * pMap.Tileset.Size, y * pMap.Tileset.Size);
				HGF::Vector2 tileMax = HGF::Vector2((x + 1) * pMap.Tileset.Size, (y + 1) * pMap.Tileset.Size);

				HGF::Vector2 translate;
				if (IsCollidingWithResponse(pPlayer.Position, pPlayer.Position + pPlayer.Dimensions, tileMin, tileMax, translate))
				{
					if (translate.X != 0.0f)
					{
						pPlayer.Position.X += translate.X;
						pPlayer.Velocity.X = 0.0f;
					}
				}
			}
		}
	}

	pPlayer.Position.Y += pPlayer.Velocity.Y;

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			if (pMap.Data[x * pMap.Height + y] >= 0)
			{
				HGF::Vector2 tileMin = HGF::Vector2(x * pMap.Tileset.Size, y * pMap.Tileset.Size);
				HGF::Vector2 tileMax = HGF::Vector2((x + 1) * pMap.Tileset.Size, (y + 1) * pMap.Tileset.Size);

				HGF::Vector2 translate;
				if (IsCollidingWithResponse(pPlayer.Position, pPlayer.Position + pPlayer.Dimensions, tileMin, tileMax, translate))
				{
					if (translate.Y != 0.0f)
					{
						pPlayer.Position.Y += translate.Y;
						pPlayer.Velocity.Y = 0.0f;
						if (translate.Y < 0.0f)
						{
							pPlayer.IsGrounded = true;
						}
					}
				}
			}
		}
	}
}

struct HitType
{
	bool IsHit;
	int TileX;
	int TileY;
	int PixelX;
	int PixelY;
	int Type;
};

bool CheckPoint(const Map& pMap, const HGF::Vector2& pPosition, HitType& pHitType)
{
	pHitType.IsHit = false;
	pHitType.TileX = (int)roundf(pPosition.X) / pMap.Tileset.Size;
	pHitType.TileY = (int)roundf(pPosition.Y) / pMap.Tileset.Size;
	pHitType.PixelX = (int)roundf(pPosition.X) % pMap.Tileset.Size;
	pHitType.PixelY = (int)roundf(pPosition.Y) % pMap.Tileset.Size;

	if (pHitType.TileX < 0 || pHitType.TileX >= pMap.Width || pHitType.TileX < 0 || pHitType.TileX >= pMap.Height)
		return false;

	pHitType.Type = pMap.Data[pHitType.TileX * pMap.Height + pHitType.TileY];
	pHitType.IsHit = (pHitType.Type >= 0);

	return pHitType.IsHit;
}

bool CheckPoint(const Map& pMap, const HGF::Vector2& pPosition)
{
	HitType hit;
	return CheckPoint(pMap, pPosition, hit);
}

void HandleWorldCollisionViaPoints(Player& pPlayer, const Map& pMap)
{
	// NOTE: this is really dumb, I'll fix later

	pPlayer.Position.X += pPlayer.Velocity.X;

	for (int i = 0; i < MRK_MAX_MARKER_COUNT; ++i)
		pPlayer.CollisionMarkers[i].IsTouching = CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[i].Position);

	// left collision
	for (int i = 3; i <= 5; ++i)
	{
		// 3 = MRK_LEFT_TOP
		// 4 = MRK_LEFT_CENTER
		// 5 = MRK_LEFT_BOTTOM
		if (pPlayer.CollisionMarkers[i].IsTouching)
		{
			float offset = 1.0f;
			while (CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[i].Position + HGF::Vector2(offset, 0.0f)))
			{
				offset += 1.0f;
			}

			pPlayer.Position.X += offset;
			pPlayer.Velocity.X = 0.0f;
			break;
		}
	}

	// right collision
	for (int i = 6; i <= 8; ++i)
	{
		// 6 = MRK_RIGHT_TOP
		// 7 = MRK_RIGHT_CENTER
		// 8 = MRK_RIGHT_BOTTOM
		if (pPlayer.CollisionMarkers[i].IsTouching)
		{
			float offset = -1.0f;
			while (CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[i].Position + HGF::Vector2(offset, 0.0f)))
			{
				offset -= 1.0f;
			}

			pPlayer.Position.X += offset;
			pPlayer.Velocity.X = 0.0f;
			break;
		}
	}

	pPlayer.Position.Y += pPlayer.Velocity.Y;

	for (int i = 0; i < MRK_MAX_MARKER_COUNT; ++i)
		pPlayer.CollisionMarkers[i].IsTouching = CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[i].Position);

	// bottom/top collision
	if (pPlayer.Velocity.Y > 0.0f)
	{
		bool hit = false;
		if (pPlayer.CollisionMarkers[MRK_HOTSPOT].IsTouching)
		{
			HitType hitType;
			if (CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[MRK_HOTSPOT].Position, hitType))
			{
				pPlayer.Position.Y = (hitType.TileY - 2) * pMap.Tileset.Size - 1;
				pPlayer.Velocity.Y = 0.0f;
				hit = true;
			}
		}
		else
		{
			for (int i = 1; i <= 2; ++i)
			{
				// 1 = MRK_FOOT_LEFT
				// 2 = MRK_FOOT_RIGHT
				HitType hitType;
				if (CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[i].Position, hitType))
				{
					pPlayer.Position.Y = (hitType.TileY - 2) * pMap.Tileset.Size - 1;
					pPlayer.Velocity.Y = 0.0f;
					hit = true;
				}
			}
		}
		pPlayer.IsGrounded = hit;
	}
	else if (pPlayer.Velocity.Y < 0.0f)
	{
		for (int i = 9; i <= 10; ++i)
		{
			// 9 = MRK_HEAD_LEFT
			// 10 = MRK_HEAD_RIGHT
			if (pPlayer.CollisionMarkers[i].IsTouching)
			{
				float offset = 1.0f;
				while (CheckPoint(pMap, pPlayer.Position + pPlayer.CollisionMarkers[i].Position + HGF::Vector2(0.0f, offset)))
				{
					offset += 1.0f;
				}

				pPlayer.Position.Y += offset;
				pPlayer.Velocity.Y = 0.0f;
				break;
			}
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
	map.Width = 64;
	map.Height = 48;
	map.Data = new int[map.Width * map.Height];
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
	player.CollisionMarkers = new CollisionMarker[MRK_MAX_MARKER_COUNT];
	for (int i = 0; i < MRK_MAX_MARKER_COUNT; ++i)
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
			printf("FPS: %d\n", frames);
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
		if (!player.IsGrounded)
		{
			player.Acceleration.Y += player.Gravity;
		}
		player.Velocity += player.Acceleration;
		player.Velocity.X *= 0.75f;
		player.Acceleration *= 0.75f;

		// handle collision
		//HandleWorldCollisionViaAABB(player, map);
		HandleWorldCollisionViaPoints(player, map);

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
	delete [] player.CollisionMarkers;

	// clean up
	SDL_DestroyTexture(player.Texture);
	SDL_DestroyTexture(map.Tileset.Texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}