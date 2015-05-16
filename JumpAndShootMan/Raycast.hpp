// Raycast.hpp

#ifndef __RAYCAST_HPP__
#define __RAYCAST_HPP__

// HGF Includes
#include <HGF\Vector2.hpp>

struct Ray
{
	HGF::Vector2 Position;
	HGF::Vector2 Direction;
	int Mask;
};

struct RaycastData
{
	HGF::Vector2 StartPosition;
	HGF::Vector2 EndPosition;
	HGF::Vector2 Direction;
	float Distance;
	HGF::Vector2 Normal;
	bool IsValid;
};

#endif