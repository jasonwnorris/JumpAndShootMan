#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "Vector2.hpp"

class Map;
class Player;
class Renderer
{
	public:
		Renderer();
		~Renderer();

		int RenderPoint(HGF::Vector2 pPosition, float pSize = 1.0f, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f) const;
		int RenderLine(HGF::Vector2 pStart, HGF::Vector2 pEnd, float pSize = 1.0f, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f) const;
		int RenderRectangle(HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pOrigin, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f) const;
		int RenderTexture(unsigned int pTextureID, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pOrigin, HGF::Vector2 pClipMin, HGF::Vector2 pClipMax, bool pFlip = false) const;
};

#endif