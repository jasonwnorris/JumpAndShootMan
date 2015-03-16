#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "Camera.hpp"

class Map;
class Player;
class Renderer
{
	public:
		Renderer();
		~Renderer();

		unsigned int LoadTexture(const char* pFilename);
		int RenderPoint(const Camera& pCamera, HGF::Vector2 pPosition, float pSize = 1.0f, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f);
		int RenderRectangle(const Camera& pCamera, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, float pRed = 1.0f, float pGreen = 1.0f, float pBlue = 1.0f, float pAlpha = 1.0f);
		int RenderTexture(const Camera& pCamera, unsigned int pTextureID, HGF::Vector2 pPosition, HGF::Vector2 pDimensions, HGF::Vector2 pClipMin, HGF::Vector2 pClipMax, bool pFlip = false);
		int RenderPlayer(const Camera& pCamera, const Player& pPlayer);
		int RenderMap(const Camera& pCamera, const Map& pMap);
};

#endif