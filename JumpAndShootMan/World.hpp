#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "Map.hpp"
#include "Player.hpp"

struct HitData
{
	int TileX;
	int TileY;
	int PixelX;
	int PixelY;
};

class World
{
	public:
		World();
		~World();

		void HandleWorldCollisionViaPoints(Player& pPlayer, const Map& pMap);
		bool CheckPoint(const Map& pMap, int pX, int pY, HitData& pHitData);
};

#endif