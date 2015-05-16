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
	// Delete all entities.
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { delete entity; });
	mEntities.clear();
}

void EntityManager::Update(float pDeltaTime)
{
	// Remove entities marked as dead.
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

	// Push in new entities.
	std::for_each(mEntitiesToAdd.begin(), mEntitiesToAdd.end(), [&](Entity* entity) { mEntities.push_back(entity); });
	mEntitiesToAdd.clear();

	// Update active entities.
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { entity->Update(pDeltaTime); });
}

void EntityManager::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	// Render entity sprites.
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { entity->Render(pSpriteBatch); });
}

void EntityManager::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	// Render entity primitives.
	std::for_each(mEntities.begin(), mEntities.end(), [&](Entity* entity) { entity->Render(pGeometryBatch); });
}