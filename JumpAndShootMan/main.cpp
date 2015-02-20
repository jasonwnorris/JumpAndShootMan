#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include "BitArray.hpp"
#include "Vector2.hpp"

#include <stdio.h>
#include <math.h>
#include <map>
#include <time.h>
#include <vector>

struct Player
{
	HGF::Vector2 Position;
	HGF::Vector2 Velocity;
	HGF::Vector2 Acceleration;
	float MovementSpeed;
	float JumpingSpeed;
	float Gravity;
	HGF::Vector2 Dimensions;
	bool IsGrounded;
	bool IsJumping;
	SDL_Texture* Texture;
};

SDL_Texture* LoadTexture(SDL_Renderer* pRenderer, const char* pFilename)
{
	SDL_Surface* surface = IMG_Load(pFilename);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	SDL_FreeSurface(surface);

	return texture;
}

int DrawTexture(SDL_Renderer* pRenderer, SDL_Texture* pTexture, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pClipLocation, HGF::Vector2 pClipSize)
{
	SDL_Rect source = { (int)pClipLocation.X, (int)pClipLocation.Y, (int)pClipSize.X, (int)pClipSize.Y };
	SDL_Rect destination = { (int)pPosition.X, (int)pPosition.Y, (int)pDimensions.X, (int)pDimensions.Y };

	return SDL_RenderCopy(pRenderer, pTexture, &source, &destination);
}

int DrawBounds(SDL_Renderer* pRenderer, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, Uint8 pRed = 255, Uint8 pGreen = 255, Uint8 pBlue = 255)
{
	SDL_Point points[5] = {
		{ (int)pPosition.X, (int)pPosition.Y },
		{ (int)(pPosition.X + pDimensions.X), (int)pPosition.Y },
		{ (int)(pPosition.X + pDimensions.X), (int)(pPosition.Y + pDimensions.Y) },
		{ (int)pPosition.X, (int)(pPosition.Y + pDimensions.Y) },
		{ (int)pPosition.X, (int)pPosition.Y }
	};

	SDL_SetRenderDrawColor(pRenderer, pRed, pGreen, pBlue, 255);
	if (SDL_RenderDrawLines(pRenderer, points, 5) < 0)
		return -1;

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

void HandleWorldCollisionForPlayer(Player& pPlayer, int* pMap, int pWidth, int pHeight, int pSize)
{
	bool horizontalMovement = pPlayer.Velocity.X < 0.0f || pPlayer.Velocity.X > 0.0f;
	bool verticalMovement = pPlayer.Velocity.Y < 0.0f || pPlayer.Velocity.Y > 0.0f;

	float minX = fminf(pPlayer.Position.X, pPlayer.Position.X + pPlayer.Velocity.X);
	float maxX = fmaxf(pPlayer.Position.X + pPlayer.Dimensions.X + pPlayer.Velocity.X, pPlayer.Position.X + pPlayer.Dimensions.X);
	float minY = fminf(pPlayer.Position.Y, pPlayer.Position.Y + pPlayer.Velocity.Y);
	float maxY = fmaxf(pPlayer.Position.Y + pPlayer.Dimensions.Y + pPlayer.Velocity.Y, pPlayer.Position.Y + pPlayer.Dimensions.Y);

	int startX = (int)minX / pSize;
	int endX = (int)maxX / pSize;
	int startY = (int)minY / pSize;
	int endY = (int)maxY / pSize;

	pPlayer.Position.X += pPlayer.Velocity.X;

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			if (pMap[x * pHeight + y] != 0)
			{
				HGF::Vector2 tileMin = HGF::Vector2(x * pSize, y * pSize);
				HGF::Vector2 tileMax = HGF::Vector2((x + 1) * pSize, (y + 1) * pSize);

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
			if (pMap[x * pHeight + y] != 0)
			{
				HGF::Vector2 tileMin = HGF::Vector2(x * pSize, y * pSize);
				HGF::Vector2 tileMax = HGF::Vector2((x + 1) * pSize, (y + 1) * pSize);

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

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return -1;

	SDL_Window* window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_SHOWN);
	if (window == NULL)
		return -1;
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		return -1;

	SDL_Texture* texture = LoadTexture(renderer, "data/img/tile.png");
	if (texture == NULL)
		return -1;

	SDL_Event evt;
	std::map<SDL_Keycode, bool> keys;
	bool running = true;
	int frames = 0;
	Uint32 prevTicks = 0;
	Uint32 currTicks = SDL_GetTicks();

	Player player;
	player.Position = HGF::Vector2(100.0f, 100.0f);
	player.Velocity = HGF::Vector2::Zero;
	player.Acceleration = HGF::Vector2::Zero;
	player.MovementSpeed = 0.75f;
	player.JumpingSpeed = 3.5f;
	player.Gravity = 0.15f;
	player.Dimensions = HGF::Vector2(70.0f, 92.0f);
	player.IsGrounded = false;
	player.IsJumping = false;
	player.Texture = LoadTexture(renderer, "data/img/player.png");
	if (player.Texture == NULL)
		return -1;

	const int width = 32;
	const int height = 24;
	const int size = 32;
	int map[width * height];
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int* tile = &map[x*height + y];

			if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
				*tile = 1;
			else if (rand() % 20 == 0)
				*tile = 1;
			else
				*tile = 0;
		}
	}

	BitArray bits(32);
	bits.Set(4, true);
	bool tempA = bits.Get(4);
	bool tempB = bits.Get(5);

	std::vector<bool> vits(32);
	vits[4] = true;

	while (running)
	{
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

		// handle input
		if (keys[SDLK_ESCAPE])
			running = false;

		/*
		if (keys[SDLK_UP])
			player.Acceleration.Y -= player.Speed;
		if (keys[SDLK_DOWN])
			player.Acceleration.Y += player.Speed;
		*/
		if (keys[SDLK_SPACE] && player.IsGrounded)
		{
			player.Acceleration.Y -= player.JumpingSpeed;
			player.IsGrounded = false;
		}
		if (keys[SDLK_LEFT])
			player.Acceleration.X -= player.MovementSpeed;
		if (keys[SDLK_RIGHT])
			player.Acceleration.X += player.MovementSpeed;

		// update movement
		player.Acceleration.Y += player.Gravity;
		player.Velocity += player.Acceleration;
		player.Velocity.X *= 0.75f;
		player.Acceleration *= 0.75f;

		// handle collision
		HandleWorldCollisionForPlayer(player, map, width, height, size);

		//printf("X: %.2f | Y: %.2f\n", player.Velocity.X, player.Velocity.Y);

		// render loop
		SDL_SetRenderDrawColor(renderer, 150, 175, 225, 255);
		if (SDL_RenderClear(renderer) < 0)
			return -1;

		if (DrawTexture(renderer, player.Texture, player.Position, player.Dimensions, HGF::Vector2::Zero, HGF::Vector2(140.0f, 184.0f)) < 0)
			return -1;
		//if (DrawBounds(renderer, player.Position, player.Dimensions, 255, 0, 255) < 0)
		//	return -1;

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				if (map[x * height + y] == 1)
				{
					if (DrawTexture(renderer, texture, HGF::Vector2(x * size, y * size), HGF::Vector2(size, size), HGF::Vector2::Zero, HGF::Vector2(64.0f, 64.0f)) < 0)
						return -1;
				}
			}
		}

		SDL_RenderPresent(renderer);

		SDL_Delay(10);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}