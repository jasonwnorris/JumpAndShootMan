// Renderer.hpp

#include "Globals.hpp"
// SDL Includes
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_opengl.h>
// Project Includes
#include "TiledMap.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

int Renderer::RenderPoint(HGF::Vector2 pPosition, float pSize, float pRed, float pGreen, float pBlue, float pAlpha) const
{
	glPointSize(pSize);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_POINTS);
		glVertex2f(pPosition.X, pPosition.Y);
	glEnd();

	Globals::DrawCount++;

	return 0;
}

int Renderer::RenderLine(HGF::Vector2 pStart, HGF::Vector2 pEnd, float pSize, float pRed, float pGreen, float pBlue, float pAlpha) const
{
	glLineWidth(pSize);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_LINES);
		glVertex2f(pStart.X, pStart.Y);
		glVertex2f(pEnd.X, pEnd.Y);
	glEnd();

	Globals::DrawCount++;

	return 0;
}

int Renderer::RenderRectangle(HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pOrigin, float pRed, float pGreen, float pBlue, float pAlpha) const
{
	HGF::Vector2 points[4] = {
		HGF::Vector2(pPosition.X - pOrigin.X,				  pPosition.Y - pOrigin.Y),
		HGF::Vector2(pPosition.X + pDimensions.X - pOrigin.X, pPosition.Y - pOrigin.Y),
		HGF::Vector2(pPosition.X + pDimensions.X - pOrigin.X, pPosition.Y + pDimensions.Y - pOrigin.Y),
		HGF::Vector2(pPosition.X - pOrigin.X,				  pPosition.Y + pDimensions.Y - pOrigin.Y)
	};

	glLineWidth(1.0f);
	glColor4f(pRed, pGreen, pBlue, pAlpha);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; ++i)
		glVertex2f(points[i].X, points[i].Y);
	glEnd();

	Globals::DrawCount++;

	return 0;
}

int Renderer::RenderTexture(const HGF::Texture& pTexture, const HGF::Vector2& pPosition, const HGF::Vector2& pDimensions, const HGF::Vector2& pOrigin, const HGF::Vector2& pClipMin, const HGF::Vector2& pClipMax, bool pFlip) const
{
	HGF::Vector2 xys[4] = {
		HGF::Vector2(pPosition.X - pOrigin.X,				  pPosition.Y - pOrigin.Y),
		HGF::Vector2(pPosition.X + pDimensions.X - pOrigin.X, pPosition.Y - pOrigin.Y),
		HGF::Vector2(pPosition.X - pOrigin.X,				  pPosition.Y + pDimensions.Y - pOrigin.Y),
		HGF::Vector2(pPosition.X + pDimensions.X - pOrigin.X, pPosition.Y + pDimensions.Y - pOrigin.Y)
	};

	HGF::Vector2 uvs[4] = {
		HGF::Vector2((pFlip ? pClipMax.X : pClipMin.X) / (float)pTexture.GetWidth(), pClipMin.Y / (float)pTexture.GetHeight()),
		HGF::Vector2((pFlip ? pClipMin.X : pClipMax.X) / (float)pTexture.GetWidth(), pClipMin.Y / (float)pTexture.GetHeight()),
		HGF::Vector2((pFlip ? pClipMax.X : pClipMin.X) / (float)pTexture.GetWidth(), pClipMax.Y / (float)pTexture.GetHeight()),
		HGF::Vector2((pFlip ? pClipMin.X : pClipMax.X) / (float)pTexture.GetWidth(), pClipMax.Y / (float)pTexture.GetHeight())
	};

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pTexture.GetID());
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < 4; ++i)
	{
		glTexCoord2f(uvs[i].X, uvs[i].Y);
		glVertex2f(xys[i].X, xys[i].Y);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	Globals::DrawCount++;

	return 0;
}