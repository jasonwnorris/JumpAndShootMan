// IMap.cpp

// Project Includes
#include "IMap.hpp"

void IMap::Raycast(const Ray& pRay, RaycastData& pRaycastData) const
{
	Raycast(pRay.Position, pRay.Direction, pRay.Mask, pRaycastData);
}