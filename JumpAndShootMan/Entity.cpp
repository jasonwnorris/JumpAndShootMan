// Entity.cpp

// Project Includes
#include "Entity.hpp"

Entity::Entity(EntityManager* pManager)
{
	mManager = pManager;
	mIsAlive = true;
}

Entity::~Entity()
{
	mManager = nullptr;
}

bool Entity::IsAlive() const
{
	return mIsAlive;
}