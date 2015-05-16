// IMap.hpp

#ifndef __IMAP_HPP__
#define __IMAP_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// Project Includes
#include "Raycast.hpp"

class IMap
{
	public:
		IMap() { }
		virtual ~IMap() { }

		virtual bool Load(const std::string& pFilename) = 0;
		void Raycast(const Ray& pRay, RaycastData& pRaycastData) const;
		virtual void Raycast(const HGF::Vector2& pPosition, const HGF::Vector2& pDirection, int pLayerMasks, RaycastData& pRaycastData) const = 0;
		virtual void Update(float pDeltaTime) = 0;
		virtual void Render(SAGE::SpriteBatch& pSpriteBatch) = 0;
		virtual void Render(SAGE::GeometryBatch& pGeometryBatch) { }
};

#endif