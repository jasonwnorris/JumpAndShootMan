// Entity.hpp

#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Timer.hpp>

class World;
class EntityManager;
class Entity
{
	friend class EntityManager;

	public:
		virtual void Update(float pDeltaTime) = 0;
		virtual void Render(SAGE::SpriteBatch& pSpriteBatch) = 0;
		virtual void Render(SAGE::GeometryBatch& pGeometryBatch);

		bool IsAlive() const;

	protected:
		Entity(EntityManager* pManager, World* pWorld);
		~Entity();

		EntityManager* mManager;
		World* mWorld;
		bool mIsAlive;
};

#endif