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

		template<typename T> std::shared_ptr<T> Create();

		void Update(float pDeltaTime);
		void Render(SAGE::SpriteBatch& pSpriteBatch);

	private:
		std::vector<std::shared_ptr<Entity>> mEntities;
};

template<typename T> std::shared_ptr<T> EntityManager::Create()
{
	static_assert(std::is_base_of<Entity, T>::value, "Invalid Type");

	std::shared_ptr<T> entity = std::make_shared<T>(this);

	mEntities.push_back(entity);

	return entity;
}

#endif