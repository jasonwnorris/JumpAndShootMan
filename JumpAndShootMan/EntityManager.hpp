// EntityManager.hpp

#ifndef __ENTITYMANAGER_HPP__
#define __ENTITYMANAGER_HPP__

// Project Includes
#include "Entity.hpp"
// STL Includes
#include <vector>

class EntityManager
{
	public:
		EntityManager();
		~EntityManager();

		template<typename T> T* Create();

		void Update(float pDeltaTime);
		void Render(const Renderer& pRenderer);
		void Render(SAGE::SpriteBatch& pSpriteBatch);

	private:
		bool RemoveDead(Entity* pEntity);

		std::vector<Entity*> mEntities;
};

template<typename T> T* EntityManager::Create()
{
	static_assert(std::is_base_of<Entity, T>::value, "Invalid Type");

	T* entity = new T(this);

	mEntities.push_back(entity);

	return entity;
}

#endif