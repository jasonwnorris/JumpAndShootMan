// TiledMap.hpp

#ifndef __TILEDMAP_HPP__
#define __TILEDMAP_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
#include <SAGE\Orientation.hpp>
// HGF Includes
#include <HGF\Texture.hpp>
// Project Includes
#include "BitField.hpp"
#include "Raycast.hpp"
// STL Includes
#include <memory>
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

class TiledMap
{
	public:
		TiledMap();
		~TiledMap();

		bool Load(const std::string& pFilename);

		bool TryGetTile(int pX, int pY, Tile& pTile) const;
		bool IsEnterable(const Tile& pTile, Direction pDirection, bool pHasInterest) const;
		bool IsTraversable(const Tile& pTile, int pPixelX, int pPixelY) const;
		void Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit) const;

		void Render(SAGE::SpriteBatch& pSpriteBatch);
		void Render(SAGE::GeometryBatch& pGeometryBatch);
		
	private:
		Tileset mTileset;
		int mWidth;
		int mHeight;
		Tile** mData;
		int mBitMaskCount;
		std::vector<std::unique_ptr<BitField>> mBitMasks;
		bool mIsLoaded;
};

#endif