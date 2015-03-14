#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_opengl.h>

#include "BitArray.hpp"
#include "Vector2.hpp"

#include <iostream>
#include <map>
#include <time.h>
#include <vector>

const float EPSILON = 0.0001f;
const int MAX_MARKER_COUNT = 12;

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
	MRK_HEAD_CENTER,
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
	unsigned int TextureID;
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
	unsigned int TextureID;
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
	Tile** Data;
	int BitMaskCount;
	bool*** BitMasks;
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
		// Establish bitmask count.
		pMap.BitMaskCount = surface->w / pMap.Tileset.Size;
		pMap.BitMasks = new bool**[pMap.BitMaskCount];

		for (int i = 0; i < pMap.BitMaskCount; ++i)
		{
			pMap.BitMasks[i] = new bool*[pMap.Tileset.Size];

			for (int px = 0; px < pMap.Tileset.Size; ++px)
			{
				pMap.BitMasks[i][px] = new bool[pMap.Tileset.Size];

				for (int py = 0; py < pMap.Tileset.Size; ++py)
				{
					Uint32 color = GetPixel(surface, i * pMap.Tileset.Size + px, 1 * pMap.Tileset.Size + py);
					Uint8 r, g, b, a;
					SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);

					pMap.BitMasks[i][px][py] = (r == 0 && g == 0 && b == 0);
				}
			}
		}

		SDL_FreeSurface(surface);
	}

	pMap.Data = new Tile*[pMap.Width];

	for (int x = 0; x < pMap.Width; ++x)
	{
		pMap.Data[x] = new Tile[pMap.Height];

		for (int y = 0; y < pMap.Height; ++y)
		{
			Tile* tile = &pMap.Data[x][y];

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

void FreeMap(Map& pMap)
{
	// Free map data.
	for (int x = 0; x < pMap.Width; ++x)
	{
		delete [] pMap.Data[x];
	}
	delete [] pMap.Data;

	// Free map bitmasks.
	for (int i = 0; i < pMap.BitMaskCount; ++i)
	{
		for (int x = 0; x < pMap.Tileset.Size; ++x)
		{
			delete [] pMap.BitMasks[i][x];
		}
		delete [] pMap.BitMasks[i];
	}
	delete [] pMap.BitMasks;
}

unsigned int LoadTexture(const char* pFilename)
{
	unsigned int id = -1;
	SDL_Surface* surface = IMG_Load(pFilename);
	if (surface != NULL)
	{
		int mode = surface->format->BitsPerPixel == 32 ? GL_RGBA : GL_RGB;

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		SDL_FreeSurface(surface);
	}

	return id;
}

int RenderPoint(const Camera& pCamera, HGF::Vector2 pPosition, float pSize = 1.0f, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f)
{
	glPointSize(pSize);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_POINTS);
		glVertex2f((pPosition.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom);
	glEnd();

	return 0;
}

int RenderRectangle(const Camera& pCamera, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f)
{
	HGF::Vector2 points[4] = {
		HGF::Vector2((pPosition.X + pCamera.Position.X) * pCamera.Zoom,					(pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pCamera.Position.X) * pCamera.Zoom,					(pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom)
	};

	glLineWidth(1.0f);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; ++i)
		glVertex2f(points[i].X, points[i].Y);
	glEnd();

	return 0;
}

int RenderTexture(const Camera& pCamera, unsigned int pTextureID, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pClipMin, HGF::Vector2 pClipMax, bool pFlip = false)
{
	HGF::Vector2 xys[4] = {
		HGF::Vector2((pPosition.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom)
	};

	HGF::Vector2 uvs[4];
	if (pFlip)
	{
		uvs[0] = HGF::Vector2(pClipMax.X, pClipMin.Y);
		uvs[1] = HGF::Vector2(pClipMin.X, pClipMin.Y);
		uvs[2] = HGF::Vector2(pClipMax.X, pClipMax.Y);
		uvs[3] = HGF::Vector2(pClipMin.X, pClipMax.Y);
	}
	else
	{
		uvs[0] = HGF::Vector2(pClipMin.X, pClipMin.Y);
		uvs[1] = HGF::Vector2(pClipMax.X, pClipMin.Y);
		uvs[2] = HGF::Vector2(pClipMin.X, pClipMax.Y);
		uvs[3] = HGF::Vector2(pClipMax.X, pClipMax.Y);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pTextureID);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; ++i)
	{
		glTexCoord2f(uvs[i].X, uvs[i].Y);
		glVertex2f(xys[i].X, xys[i].Y);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	return 0;
}

int RenderPlayer(const Camera& pCamera, const Player& pPlayer)
{
	// texture
	if (RenderTexture(pCamera, pPlayer.TextureID, pPlayer.Position, pPlayer.Dimensions, HGF::Vector2::Zero, HGF::Vector2::One, pPlayer.IsFacingLeft) < 0)
		return -1;

	// AABB
	if (RenderRectangle(pCamera, pPlayer.Position, pPlayer.Dimensions, 0.0f, 1.0f, 0.0f) < 0)
		return -1;

	// collision points
	for (int i = 0; i < MAX_MARKER_COUNT; ++i)
	{
		float r = pPlayer.CollisionMarkers[i].IsTouching ? 150.0f / 255.0f : 50.0f / 255.0f;
		float g = pPlayer.CollisionMarkers[i].IsTouching ? 175.0f / 255.0f : 75.0f / 255.0f;
		float b = pPlayer.CollisionMarkers[i].IsTouching ? 180.0f / 255.0f : 80.0f / 255.0f;
		if (RenderPoint(pCamera, pPlayer.Position + pPlayer.CollisionMarkers[i].Position, 4.0f, r, g, b) < 0)
			return -1;
	}

	return 0;
}

int RenderMap(const Camera& pCamera, const Map& pMap)
{
	for (int y = 0; y < pMap.Height; ++y)
	{
		for (int x = 0; x < pMap.Width; ++x)
		{			
			int value = pMap.Data[x][y].TextureID;
			if (value >= 0)
			{
				HGF::Vector2 position(x * pMap.Tileset.Size, y * pMap.Tileset.Size);
				HGF::Vector2 dimensions(pMap.Tileset.Size, pMap.Tileset.Size);
				HGF::Vector2 min((value % pMap.Tileset.Y) / (float)pMap.Tileset.X, (value / pMap.Tileset.Y) / (float)pMap.Tileset.Y);
				HGF::Vector2 max(((value % pMap.Tileset.Y) + 1) / (float)pMap.Tileset.X, ((value / pMap.Tileset.Y) + 1) / (float)pMap.Tileset.Y);

				if (RenderTexture(pCamera, pMap.Tileset.TextureID, position, dimensions, min, max) < 0)
					return -1;
			}
		}
	}

	return 0;
}

bool CheckPoint(const Map& pMap, int pX, int pY, HitData& pHitData)
{
	// Get the tile coordinates.
	pHitData.TileX = pX / pMap.Tileset.Size;
	pHitData.TileY = pY / pMap.Tileset.Size;
	// Get the inter-tile pixel coordinates.
	pHitData.PixelX = pX % pMap.Tileset.Size;
	pHitData.PixelY = pY % pMap.Tileset.Size;

	// Check if within map bounds.
	if (pHitData.TileX < 0 || pHitData.TileX >= pMap.Width || pHitData.TileY < 0 || pHitData.TileY >= pMap.Height)
		return false;

	// Grab the tile.
	Tile* tile = &pMap.Data[pHitData.TileX][pHitData.TileY];
	if (tile->CollisionID < 0)
		return false;

	// Check against bitmask.
	return pMap.BitMasks[tile->CollisionID][pHitData.PixelX][pHitData.PixelY];
}

void HandleWorldCollisionViaPoints(Player& pPlayer, const Map& pMap)
{
	HitData hitData;
	bool hitHorz = false;
	bool hitVert = false;

	// left collision
	for (int i = 3; i <= 5; ++i)
	{
		// 3 = MRK_LEFT_CENTER
		// 4 = MRK_LEFT_TOP
		// 5 = MRK_LEFT_BOTTOM
		int start = (int)(pPlayer.Position.X + pPlayer.Dimensions.X / 2.0f);
		int finish = (int)(pPlayer.Position.X + pPlayer.Velocity.X + pPlayer.CollisionMarkers[i].Position.X);
		int y = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
		for (int x = start; x >= finish; --x)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				if (pMap.Data[hitData.TileX][hitData.TileY].CollisionID == 0)
				{
					pPlayer.Position.X = (float)(x - 3) + pPlayer.CollisionMarkers[i].Position.X;
					pPlayer.Velocity.X = 0.0f;
					hitHorz = true;
					break;
				}
			}
		}
		if (hitHorz)
			break;
	}

	// right collision
	for (int i = 6; i <= 8; ++i)
	{
		// 6 = MRK_RIGHT_CENTER
		// 7 = MRK_RIGHT_TOP
		// 8 = MRK_RIGHT_BOTTOM
		int start = (int)(pPlayer.Position.X + pPlayer.Dimensions.X / 2.0f);
		int finish = (int)(pPlayer.Position.X + pPlayer.Velocity.X + pPlayer.CollisionMarkers[i].Position.X);
		int y = (int)(pPlayer.Position.Y + pPlayer.CollisionMarkers[i].Position.Y);
		for (int x = start; x <= finish; ++x)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				if (pMap.Data[hitData.TileX][hitData.TileY].CollisionID == 0)
				{
					pPlayer.Position.X = (float)x- pPlayer.CollisionMarkers[i].Position.X;
					pPlayer.Velocity.X = 0.0f;
					hitHorz = true;
					break;
				}
			}
		}
		if (hitHorz)
			break;
	}
	
	// progress with horizontal movement if no collision
	if (!hitHorz)
		pPlayer.Position.X += pPlayer.Velocity.X;

	// bottom collision
	for (int i = 0; i <= 2; ++i)
	{
		// 0 = MRK_HOTSPOT
		// 1 = MRK_FOOT_LEFT
		// 2 = MRK_FOOT_RIGHT
		int start = (int)(pPlayer.Position.Y + pPlayer.Dimensions.Y / 2.0f);
		int finish = (int)(pPlayer.Position.Y + pPlayer.Velocity.Y + pPlayer.CollisionMarkers[i].Position.Y);
		int x = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
		for (int y = start; y <= finish; ++y)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				int id = pMap.Data[hitData.TileX][hitData.TileY].CollisionID;
				if (!((i == 1 || i == 2) && id != 0))
				{
					if (pMap.BitMasks[id][hitData.PixelX][hitData.PixelY])
					{
						pPlayer.Position.Y = (float)y - pPlayer.CollisionMarkers[i].Position.Y;
						pPlayer.Velocity.Y = 0.0f;
						hitVert = true;
						pPlayer.IsGrounded = true;
						break;
					}
				}
			}
		}
		if (hitVert)
			break;
	}

	// top collision
	for (int i = 9; i <= 11; ++i)
	{
		// 9 = MRK_HEAD_CENTER
		// 10 = MRK_HEAD_LEFT
		// 11 = MRK_HEAD_RIGHT
		int start = (int)(pPlayer.Position.Y + pPlayer.Dimensions.Y / 2.0f);
		int finish = (int)(pPlayer.Position.Y + pPlayer.Velocity.Y + pPlayer.CollisionMarkers[i].Position.Y);
		int x = (int)(pPlayer.Position.X + pPlayer.CollisionMarkers[i].Position.X);
		for (int y = start; y >= finish; --y)
		{
			if (CheckPoint(pMap, x, y, hitData))
			{
				pPlayer.Position.Y = (float)(y + 1) + pPlayer.CollisionMarkers[i].Position.Y;
				pPlayer.Velocity.Y = 0.0f;
				hitVert = true;
				break;
			}
		}
		if (hitVert)
			break;
	}

	// progress with vertical movement if no collision
	if (!hitVert)
		pPlayer.Position.Y += pPlayer.Velocity.Y;
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return -1;

	int windowWidth = 1024;
	int windowHeight = 768;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_Window* window = SDL_CreateWindow("JUMP AND SHOOT MAN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == NULL)
		return -1;

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL)
		return -1;

	if (SDL_GL_SetSwapInterval(1) < 0)
		return -1;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	SDL_Event evt;
	std::map<SDL_Keycode, bool> keys;
	bool running = true;
	int frames = 0;
	Uint32 prevTicks = 0;
	Uint32 currTicks = SDL_GetTicks();

	Map map;
	map.Tileset.TextureID = LoadTexture("data/img/tileset2.png");
	if (map.Tileset.TextureID < 0)
		return -1;
	map.Tileset.X = 3;
	map.Tileset.Y = 3;
	map.Tileset.Size = 16;
	map.Width = 16;
	map.Height = 12;
	GenerateMap(map);

	Player player;
	player.TextureID = LoadTexture("data/img/megamanx.png");
	if (player.TextureID < 0)
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
	player.CollisionMarkers[MRK_LEFT_TOP].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 16.0f, player.Dimensions.Y * 1.0f / 6.0f);
	player.CollisionMarkers[MRK_LEFT_CENTER].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 16.0f, player.Dimensions.Y * 3.0f / 6.0f);
	player.CollisionMarkers[MRK_LEFT_BOTTOM].Position = HGF::Vector2(player.Dimensions.X * 1.0f / 16.0f, player.Dimensions.Y * 5.0f / 6.0f);
	player.CollisionMarkers[MRK_RIGHT_TOP].Position = HGF::Vector2(player.Dimensions.X * 15.0f / 16.0f, player.Dimensions.Y * 1.0f / 6.0f);
	player.CollisionMarkers[MRK_RIGHT_CENTER].Position = HGF::Vector2(player.Dimensions.X * 15.0f / 16.0f, player.Dimensions.Y * 3.0f / 6.0f);
	player.CollisionMarkers[MRK_RIGHT_BOTTOM].Position = HGF::Vector2(player.Dimensions.X * 15.0f / 16.0f, player.Dimensions.Y * 5.0f / 6.0f);
	player.CollisionMarkers[MRK_HEAD_CENTER].Position = HGF::Vector2(player.Dimensions.X * 3.0f / 6.0f, 0.0f);
	player.CollisionMarkers[MRK_HEAD_LEFT].Position = HGF::Vector2(player.Dimensions.X * 2.0f / 6.0f, 0.0f);
	player.CollisionMarkers[MRK_HEAD_RIGHT].Position = HGF::Vector2(player.Dimensions.X * 4.0f / 6.0f, 0.0f);

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
		if (!player.IsGrounded)
		{
			player.Acceleration.Y += player.Gravity;
		}
		player.Velocity += player.Acceleration;
		player.Velocity.X *= 0.75f;
		player.Acceleration *= 0.75f;

		// handle collision
		HandleWorldCollisionViaPoints(player, map);

		// focus camera
		camera.Position.X = windowWidth / 4.0f - player.Position.X - player.Dimensions.X / 2.0f;
		camera.Position.Y = windowHeight / 4.0f - player.Position.Y - player.Dimensions.Y / 2.0f;
		
		// clear
		glClearColor(150.0f / 255.0f, 175.0f / 255.0f, 225.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// render loop
		if (RenderMap(camera, map) < 0)
			return -1;
		if (RenderPlayer(camera, player) < 0)
			return -1;

		// flip
		SDL_GL_SwapWindow(window);

		SDL_Delay(10);
	}

	// free memory
	FreeMap(map);

	// delete textures
	glDeleteTextures(1, &player.TextureID);
	glDeleteTextures(1, &map.Tileset.TextureID);

	// clean up
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}