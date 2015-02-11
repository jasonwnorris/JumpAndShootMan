#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include <stdio.h>
#include <math.h>
#include <map>

struct Vector2
{
	int X;
	int Y;

	Vector2()
	{
		X = 0;
		Y = 0;
	}

	Vector2(int pX, int pY)
	{
		X = pX;
		Y = pY;
	}
};

struct KeyState
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;

	KeyState()
	{
		Up = false;
		Down = false;
		Left = false;
		Right = false;
	}

	KeyState(bool pUp, bool pDown, bool pLeft, bool pRight)
	{
		Up = pUp;
		Down = pDown;
		Left = pLeft;
		Right = pRight;
	}
};

struct Player
{
	Vector2 Position;
	Vector2 Velocity;
	Vector2 Acceleration;
	int Speed;
	Vector2 Dimensions;
};

inline int Abs(int pValue)
{
	return (pValue < 0) ? -pValue : pValue;
}

bool IsColliding(const Vector2& pMinA, const Vector2& pMaxA, const Vector2& pMinB, const Vector2& pMaxB)
{
	if (pMaxA.X < pMinB.X || pMaxA.Y < pMinB.Y || pMinA.X > pMaxB.X || pMinA.Y > pMaxB.Y)
		return false;

	return true;
}

bool IsCollidingWithResponse(const Vector2& pMinA, const Vector2& pMaxA, const Vector2& pMinB, const Vector2& pMaxB, Vector2& pTranslation)
{
	int left = pMinB.X - pMaxA.X;
	int right = pMaxB.X - pMinA.X;
	int top = pMinB.Y - pMaxA.Y;
	int bottom = pMaxB.Y - pMinA.Y;

	if (left > 0.0f || right < 0.0f || top > 0.0f || bottom < 0.0f)
		return false;

	// minimum X-axis
	pTranslation.X = (Abs(left) < Abs(right)) ? left : right;

	// minimum Y-axis
	pTranslation.Y = (Abs(top) < Abs(bottom)) ? top : bottom;

	// minimal axis
	if (Abs(pTranslation.X) < Abs(pTranslation.Y))
		pTranslation.Y = 0;
	else
		pTranslation.X = 0;

	return true;
}

SDL_Texture* LoadTexture(SDL_Renderer* pRenderer, const char* pFilename)
{
	SDL_Surface* surface = IMG_Load(pFilename);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	SDL_FreeSurface(surface);
	
	return texture;
}

int DrawTexture(SDL_Renderer* pRenderer, SDL_Texture* pTexture, int pPositionX, int pPositionY, int pWidth, int pHeight, int pClipX, int pClipY, int pClipWidth, int pClipHeight)
{
	SDL_Rect source = { pClipX, pClipY, pClipWidth, pClipHeight };
	SDL_Rect destination = { pPositionX, pPositionY, pWidth, pHeight };
	
	return SDL_RenderCopy(pRenderer, pTexture, &source, &destination);
}

int DrawBounds(SDL_Renderer* pRenderer, Vector2 pPosition, Vector2 pDimensions)
{
	SDL_Point points[5] = {
		{ pPosition.X,                 pPosition.Y },
		{ pPosition.X + pDimensions.X, pPosition.Y },
		{ pPosition.X + pDimensions.X, pPosition.Y + pDimensions.Y },
		{ pPosition.X,                 pPosition.Y + pDimensions.Y },
		{ pPosition.X,                 pPosition.Y }
	};

	SDL_SetRenderDrawColor(pRenderer, 255, 0, 255, 255);
	if (SDL_RenderDrawLines(pRenderer, points, 5) < 0)
		return -1;

	return 0;
}

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return -1;

	SDL_Window* window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
	if (window == NULL)
		return -1;
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		return -1;

	SDL_Texture* texture = LoadTexture(renderer, "data/img/player.png");
	if (texture == NULL)
		return -1;

	SDL_Event evt;
	bool running = true;

	KeyState keyState;
	Player player;
	player.Position = { 100, 100 };
	player.Velocity = { 0, 0 };
	player.Acceleration = { 0, 0 };
	player.Speed = 10;
	player.Dimensions = { 128, 128 };

	const int width = 10;
	const int height = 10;
	const int size = 64;
	int map[width * height];
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			map[x * 10 + y] = (x == 0 || x == width - 1 || y == 0 || y == height - 1) ? 1 : 0;
		}
	}

	int frames = 0;
	Uint32 prevTicks = 0;
	Uint32 currTicks = SDL_GetTicks();

	std::map<SDL_Keycode, bool> keys;

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

		// update loop
		player.Acceleration = { 0, 0 };
		if (keys[SDLK_UP])
			player.Acceleration.Y -= 5;
		if (keys[SDLK_DOWN])
			player.Acceleration.Y += 5;
		if (keys[SDLK_LEFT])
			player.Acceleration.X -= 5;
		if (keys[SDLK_RIGHT])
			player.Acceleration.X += 5;

		player.Velocity.X += player.Acceleration.X;
		if (player.Velocity.X > player.Speed)
			player.Velocity.X = player.Speed;
		else if (player.Velocity.X < -player.Speed)
			player.Velocity.X = -player.Speed;

		player.Velocity.Y += player.Acceleration.Y;
		if (player.Velocity.Y > player.Speed)
			player.Velocity.Y = player.Speed;
		else if (player.Velocity.Y < -player.Speed)
			player.Velocity.Y = -player.Speed;

		player.Position.X += player.Velocity.X;
		player.Position.Y += player.Velocity.Y;

		player.Velocity.X /= 2;
		player.Velocity.Y /= 2;

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				if (map[x * 10 + y] == 1)
				{
					Vector2 p = { player.Position.X + player.Dimensions.X, player.Position.Y + player.Dimensions.Y };
					Vector2 tileMin = { x * size, y * size };
					Vector2 tileMax = { (x + 1) * size, (y + 1) * size };

					Vector2 translate;
					if (IsCollidingWithResponse(player.Position, p, tileMin, tileMax, translate))
					{
						if (translate.X < 0 || translate.X > 0)
						{
							player.Position.X += translate.X;
							player.Velocity.X = 0;
						}
						if (translate.Y < 0 || translate.Y > 0)
						{
							player.Position.Y += translate.Y;
							player.Velocity.Y = 0;
						}
					}
				}
			}
		}

		// render loop
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		if (SDL_RenderClear(renderer) < 0)
			return -1;

		if (DrawTexture(renderer, texture, player.Position.X, player.Position.Y, player.Dimensions.X, player.Dimensions.Y, 0, 0, player.Dimensions.X, player.Dimensions.Y) < 0)
			return -1;
		if (DrawBounds(renderer, player.Position, player.Dimensions) < 0)
			return -1;

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				if (map[x * 10 + y] == 1)
				{
					if (DrawTexture(renderer, texture, x * size, y * size, size, size, 0, 0, 128, 128) < 0)
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