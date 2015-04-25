// ITiledTiledMapCollider.hpp

#ifndef __ITILEDMAPCOLLIDER_HPP__
#define __ITILEDMAPCOLLIDER_HPP__

// Project Includes
#include "TiledMap.hpp"

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

class ITiledTiledMapCollider
{
	public:
		ITiledTiledMapCollider(TiledMap* pTiledMap) { mTiledMap = pTiledMap; }
		virtual ~ITiledTiledMapCollider() { mTiledMap = nullptr; }

	//protected:
	public:
		static const int MaxRayCount = 12;
		RaycastInfo RaycastInfos[MaxRayCount];
		RaycastHit RaycastHits[MaxRayCount];

	private:
		TiledMap* mTiledMap;
};

#endif