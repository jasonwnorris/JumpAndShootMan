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

		bool Load(const std::string& p_Filename) override;
		void Raycast(const HGF::Vector2& p_Position, const HGF::Vector2& p_Direction, int p_LayerMasks, RaycastData& p_RaycastData) const override;
		void Update(float p_DeltaTime) override;
		void Render(SAGE::SpriteBatch& p_SpriteBatch) override;
		void Render(SAGE::GeometryBatch& p_GeometryBatch) override;
		
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

		bool TryGetTile(int p_X, int p_Y, Tile& p_Tile) const;
		bool IsEnterable(const Tile& p_Tile, EdgeSide p_Edge, int p_Mask) const;
		bool IsTraversable(const Tile& p_Tile, int p_PixelX, int p_PixelY) const;

		Tileset m_Tileset;
		int m_Width;
		int m_Height;
		std::vector<std::vector<Tile>> m_Data;
		int m_BitMaskCount;
		std::vector<BitField> m_BitMasks;
};

#endif
