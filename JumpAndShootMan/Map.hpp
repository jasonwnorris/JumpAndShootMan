#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "Texture.hpp"
#include "Vector2.hpp"

#include <string>

struct Tileset
{
	HGF::Texture Texture;
	int X;
	int Y;
	int Size;
};

struct Tile
{
	int TextureID;
	int CollisionID;
};

class Renderer;
class Map
{
	public:
		Map();
		~Map();

		bool Load(const std::string& pFilename);
		bool Raycast(const HGF::Vector2& pPosition, const HGF::Vector2& pDirection, float pDistance, HGF::Vector2& pOutHit);
		bool IsTileEmpty(int pTileX, int pTileY);
		bool IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY);
		bool Render(const Renderer& pRenderer);

		Tileset Tileset;
		int Width;
		int Height;
		Tile** Data;
		int BitMaskCount;
		bool*** BitMasks;
};

#endif