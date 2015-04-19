// Map.hpp

#ifndef __MAP_HPP__
#define __MAP_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
#include <SAGE\Orientation.hpp>
// HGF Includes
#include <HGF\Texture.hpp>
// Project Includes
#include "BitField.hpp"
#include "Renderer.hpp"
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
	SAGE::Orientation Orientation;
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
		void Render(const Renderer& pRenderer);
		void RenderDebug(const Renderer& pRenderer);
		void Render(SAGE::SpriteBatch& pSpriteBatch);
		void RenderDebug(SAGE::GeometryBatch& pGeometryBatch);
		
	private:
		Tileset mTileset;
		int mWidth;
		int mHeight;
		Tile** mData;
		int mBitMaskCount;
		BitField* mBitMasks;
		bool mIsLoaded;
};

#endif