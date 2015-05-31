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

		virtual bool Load(const std::string& p_Filename) = 0;
		void Raycast(const Ray& p_Ray, RaycastData& p_RaycastData) const;
		virtual void Raycast(const HGF::Vector2& p_Position, const HGF::Vector2& p_Direction, int p_LayerMasks, RaycastData& p_RaycastData) const = 0;
		virtual void Update(float p_DeltaTime) = 0;
		virtual void Render(SAGE::SpriteBatch& p_SpriteBatch) = 0;
		virtual void Render(SAGE::GeometryBatch& p_GeometryBatch) { }
};

#endif
