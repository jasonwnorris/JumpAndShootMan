// EntityManager.hpp

#ifndef __ENTITYMANAGER_HPP__
#define __ENTITYMANAGER_HPP__

// Project Includes
#include "Entity.hpp"
// STL Includes
#include <memory>
#include <vector>

class EntityManager
{
	public:
		EntityManager();
		~EntityManager();

		template<typename T> T* Create(World* pWorld);

		void Update(float pDeltaTime);
		void Render(SAGE::SpriteBatch& pSpriteBatch);
		void Render(SAGE::GeometryBatch& pGeometryBatch);

	private:
		std::vector<Entity*> mEntities;
		std::vector<Entity*> mEntitiesToAdd;
};

template<typename T> T* EntityManager::Create(World* pWorld)
{
	static_assert(std::is_base_of<Entity, T>::value, "Invalid Type");

	T* entity = new T(this, pWorld);

	mEntitiesToAdd.push_back(entity);

	return entity;
}

#endif