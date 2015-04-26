// EntityManager.cpp

// Project Includes
#include "EntityManager.hpp"
// STL Includes
#include <algorithm>

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{
	mEntities.clear();
}

void EntityManager::Update(float pDeltaTime)
{
	mEntities.erase(std::remove_if(mEntities.begin(), mEntities.end(), [](std::weak_ptr<Entity> entity) { return !entity.lock()->IsAlive(); }), mEntities.end());

	std::for_each(mEntities.begin(), mEntities.end(), [&](std::weak_ptr<Entity> entity) { entity.lock()->Update(pDeltaTime); });
}

void EntityManager::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	std::for_each(mEntities.begin(), mEntities.end(), [&](std::weak_ptr<Entity> entity) { entity.lock()->Render(pSpriteBatch); });
}