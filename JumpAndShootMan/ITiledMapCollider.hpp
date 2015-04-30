// ITiledTiledMapCollider.hpp

#ifndef __ITILEDMAPCOLLIDER_HPP__
#define __ITILEDMAPCOLLIDER_HPP__

// Project Includes
#include "World.hpp"

class ITiledMapCollider
{
	protected:
		ITiledMapCollider(World* pWorld);
		~ITiledMapCollider();

		void SetTriRay(Direction pDirection, HGF::Vector2& pPosition, float pSpread, float pMinimum, float pMaximum);
		void CheckCollision();
		void Render(SAGE::GeometryBatch& pGeometryBatch);

		virtual void OnCollision(Direction pDirection, float pDistance) = 0;
		virtual void OnEmpty(Direction pDirection, float pDistance) = 0;

	private:
		static const int MaxRayCount = 12;		
		enum RayIndex
		{
			DownCenter,
			DownLeft,
			DownRight,
			UpCenter,
			UpLeft,
			UpRight,
			LeftCenter,
			LeftTop,
			LeftBottom,
			RightCenter,
			RightTop,
			RightBottom,
		};

		World* mWorld;
		RaycastInfo RaycastInfos[MaxRayCount];
		RaycastHit RaycastHits[MaxRayCount];
};

#endif