// Entity.cpp

// Project Includes
#include "Entity.hpp"

Entity::Entity(EntityManager* pManager, World* pWorld)
{
	mManager = pManager;
	mWorld = pWorld;
	mIsAlive = true;
}

Entity::~Entity()
{
	mManager = nullptr;
	mWorld = nullptr;
}

void Entity::Render(SAGE::GeometryBatch& pGeometryBatch)
{
}

bool Entity::IsAlive() const
{
	return mIsAlive;
}