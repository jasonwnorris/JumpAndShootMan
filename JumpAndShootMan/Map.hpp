#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "Raycast.hpp"
#include "Texture.hpp"

#include <string>
#include <vector>

enum EdgeType
{
	Empty = 0,
	Solid = 1,
	Interesting = 2
};

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
	EdgeType Edges[4];
};

class Renderer;
class Map
{
	public:
		Map();
		~Map();

		bool Load(const std::string& pFilename);
		bool Randomize();
		bool Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit);
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