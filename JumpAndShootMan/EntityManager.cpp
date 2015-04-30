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
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { delete entity; });

	mEntities.clear();
}

void EntityManager::Update(float pDeltaTime)
{
	mEntities.erase(std::remove_if(mEntities.begin(), mEntities.end(), [](Entity* entity) {
		if (!entity->IsAlive())
		{
			delete entity;

			return true;
		}
		else
		{
			return false;
		}
	}), mEntities.end());

	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { entity->Update(pDeltaTime); });
}

void EntityManager::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { entity->Render(pSpriteBatch); });
}

void EntityManager::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { entity->Render(pGeometryBatch); });
}