#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include <stdio.h>
#include <math.h>

struct Vector2
{
	int X;
	int Y;
};

struct KeyState
{
	bool Up;
	bool Down;
	bool Left;
	bool Right;
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

	SDL_Rect source = { 0, 0, 128, 128 };
	SDL_Rect destination = { 100, 100, 128, 128 };

	SDL_Event evt;
	bool running = true;

	KeyState keyState = { false, false, false, false };
	Vector2 position = { 100, 100 };
	Vector2 other = { 400, 300 };
	Vector2 dims = { 128, 128 };
	int speed = 3;

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
					switch (evt.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = false;
							break;
						case SDLK_UP:
							keyState.Up = true;
							break;
						case SDLK_DOWN:
							keyState.Down = true;
							break;
						case SDLK_LEFT:
							keyState.Left = true;
							break;
						case SDLK_RIGHT:
							keyState.Right = true;
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
					switch (evt.key.keysym.sym)
					{
						case SDLK_UP:
							keyState.Up = false;
							break;
						case SDLK_DOWN:
							keyState.Down = false;
							break;
						case SDLK_LEFT:
							keyState.Left = false;
							break;
						case SDLK_RIGHT:
							keyState.Right = false;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		// update loop
		if (keyState.Up)
			position.Y -= speed;
		if (keyState.Down)
			position.Y += speed;
		if (keyState.Left)
			position.X -= speed;
		if (keyState.Right)
			position.X += speed;

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				if (map[x * 10 + y] == 1)
				{
					Vector2 p = { position.X + dims.X, position.Y + dims.Y };
					Vector2 tileMin = { x * size, y * size };
					Vector2 tileMax = { (x + 1) * size, (y + 1) * size };

					Vector2 translate;
					if (IsCollidingWithResponse(position, p, tileMin, tileMax, translate))
					{
						position.X += translate.X;
						position.Y += translate.Y;
					}
				}
			}
		}

		// render loop
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		if (SDL_RenderClear(renderer) < 0)
			return -1;

		if (DrawTexture(renderer, texture, position.X, position.Y, dims.X, dims.Y, 0, 0, dims.X, dims.Y) < 0)
			return -1;
		if (DrawBounds(renderer, position, dims) < 0)
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