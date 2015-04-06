// Map.hpp

#ifndef __MAP_HPP__
#define __MAP_HPP__

// HGF Includes
#include <HGF\Texture.hpp>
// Project Includes
#include "Raycast.hpp"
// STL Includes
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
		void Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit);
		bool IsTileEmpty(int pTileX, int pTileY);
		bool IsTraversable(int pTileX, int pTileY, int pPixelX, int pPixelY);
		bool Render(const Renderer& pRenderer);
		
	private:
		Tileset mTileset;
		int mWidth;
		int mHeight;
		Tile** mData;
		int mBitMaskCount;
		bool*** mBitMasks;
		bool mIsLoaded;
};

#endif