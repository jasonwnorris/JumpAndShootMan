// Raycast.hpp

#ifndef __RAYCAST_HPP__
#define __RAYCAST_HPP__

// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "Direction.hpp"

struct RaycastInfo
{
	HGF::Vector2 Position;
	Direction Direction;
	float Distance;
	float Threshold;
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