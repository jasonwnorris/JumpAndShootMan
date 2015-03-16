#include "Renderer.hpp"

#include "Map.hpp"
#include "Player.hpp"

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_opengl.h>

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

unsigned int Renderer::LoadTexture(const char* pFilename)
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

int Renderer::RenderPoint(const Camera& pCamera, HGF::Vector2 pPosition, float pSize, float pRed, float pGreen, float pBlue, float pAlpha)
{
	glPointSize(pSize);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_POINTS);
	glVertex2f((pPosition.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom);
	glEnd();

	return 0;
}

int Renderer::RenderRectangle(const Camera& pCamera, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, float pRed, float pGreen, float pBlue, float pAlpha)
{
	HGF::Vector2 points[4] = {
		HGF::Vector2((pPosition.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pDimensions.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom),
		HGF::Vector2((pPosition.X + pCamera.Position.X) * pCamera.Zoom, (pPosition.Y + pDimensions.Y + pCamera.Position.Y) * pCamera.Zoom)
	};

	glLineWidth(1.0f);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; ++i)
		glVertex2f(points[i].X, points[i].Y);
	glEnd();

	return 0;
}

int Renderer::RenderTexture(const Camera& pCamera, unsigned int pTextureID, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pClipMin, HGF::Vector2 pClipMax, bool pFlip)
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

int Renderer::RenderPlayer(const Camera& pCamera, const Player& pPlayer)
{
	// texture
	if (RenderTexture(pCamera, pPlayer.TextureID, pPlayer.Position, pPlayer.Dimensions, HGF::Vector2::Zero, HGF::Vector2::One, pPlayer.IsFacingLeft) < 0)
		return -1;

	// AABB
	if (RenderRectangle(pCamera, pPlayer.Position, pPlayer.Dimensions, 0.0f, 1.0f, 0.0f) < 0)
		return -1;

	// collision points
	for (int i = 0; i < Player::MaxMarkerCount; ++i)
	{
		float r = pPlayer.CollisionMarkers[i].IsTouching ? 150.0f / 255.0f : 50.0f / 255.0f;
		float g = pPlayer.CollisionMarkers[i].IsTouching ? 175.0f / 255.0f : 75.0f / 255.0f;
		float b = pPlayer.CollisionMarkers[i].IsTouching ? 180.0f / 255.0f : 80.0f / 255.0f;
		if (RenderPoint(pCamera, pPlayer.Position + pPlayer.CollisionMarkers[i].Position, 4.0f, r, g, b) < 0)
			return -1;
	}

	return 0;
}

int Renderer::RenderMap(const Camera& pCamera, const Map& pMap)
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