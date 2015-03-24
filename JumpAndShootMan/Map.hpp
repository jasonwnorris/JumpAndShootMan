#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "Direction.hpp"
#include "Texture.hpp"
#include "Vector2.hpp"

#include <string>
#include <vector>

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
		bool Randomize();
		bool Raycast(const HGF::Vector2& pPosition, Direction pDirection, const std::vector<int>& pTargets, HGF::Vector2& pOutHit, float& pOutDistance);
		bool IsTileEmpty(int pTileX, int pTileY);
		bool IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY);
		bool Render(const Renderer& pRenderer);

		Tileset Tileset;
		int Width;
		int Height;
		Tile** Data;
		int BitMaskCount;
		bool*** BitMasks;

	private:
		bool mIsLoaded;
};

#endif