// Entity.hpp

#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

// SAGE Includes
#include <SAGE\SpriteBatch.hpp>

class EntityManager;
class Entity
{
	friend class EntityManager;

	public:
		virtual void Update(float pDeltaTime) = 0;
		virtual void Render(SAGE::SpriteBatch& pSpriteBatch) = 0;

		bool IsAlive() const;

	protected:
		Entity(EntityManager* pManager);
		~Entity();

		EntityManager* mManager;
		bool mIsAlive;
};

#endif