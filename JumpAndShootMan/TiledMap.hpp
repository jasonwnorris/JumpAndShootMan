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
#include "IMap.hpp"
#include "Raycast.hpp"
// STL Includes
#include <string>
#include <vector>

class TiledMap : public IMap
{
	public:
		TiledMap();
		~TiledMap();

		bool Load(const std::string& pFilename) override;
		void Raycast(const HGF::Vector2& pPosition, const HGF::Vector2& pDirection, int pLayerMasks, RaycastData& pRaycastData) const override;
		void Update(float pDeltaTime) override;
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;
		void Render(SAGE::GeometryBatch& pGeometryBatch) override;
		
	private:
		enum EdgeMask
		{
			Empty = 0,
			Solid = 1,
			Interesting = 2
		};

		enum EdgeSide
		{
			Top,
			Bottom,
			Left,
			Right,
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
			EdgeMask Edges[4];
			SAGE::Orientation Orientation;
		};

		bool TryGetTile(int pX, int pY, Tile& pTile) const;
		bool IsEnterable(const Tile& pTile, EdgeSide pEdge, int pMask) const;
		bool IsTraversable(const Tile& pTile, int pPixelX, int pPixelY) const;

		Tileset mTileset;
		int mWidth;
		int mHeight;
		std::vector<std::vector<Tile>> mData;
		int mBitMaskCount;
		std::vector<BitField> mBitMasks;
};

#endif