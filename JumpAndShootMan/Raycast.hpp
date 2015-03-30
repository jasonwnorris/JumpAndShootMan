#ifndef __RAYCAST_HPP__
#define __RAYCAST_HPP__

#include "Direction.hpp"
#include "Vector2.hpp"

struct RayInfo
{
	HGF::Vector2 Position;
	Direction Direction;
	float Distance;
	bool HasInterest;
};

struct RaycastHit
{
	HGF::Vector2 Position;
	float Distance;
	int TileX;
	int TileY;
};

#endif