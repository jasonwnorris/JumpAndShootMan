// IMap.cpp

// Project Includes
#include "IMap.hpp"

void IMap::Raycast(const Ray& p_Ray, RaycastData& p_RaycastData) const
{
	Raycast(p_Ray.Position, p_Ray.Direction, p_Ray.Mask, p_RaycastData);
}
