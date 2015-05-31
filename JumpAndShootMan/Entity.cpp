// Entity.cpp

// Project Includes
#include "Entity.hpp"
#include "World.hpp"
// STL Includes
#include <algorithm>

Entity::Entity(World* p_World)
{
	m_World = p_World;
}

Entity::~Entity()
{
	m_World = nullptr;
	m_Parent = nullptr;
	m_Children.clear();
	m_Components.clear();
}

World* Entity::GetWorld() const
{
	return m_World;
}

void Entity::Destroy()
{
	IDestroyable::Destroy();

	// Destory attached components;
	for (std::map<std::type_index, IComponent*>::iterator iter = m_Components.begin(); iter != m_Components.end(); ++iter)
	{
		iter->second->Destroy();
	}

	// TODO: Remove parent attachment.

	// TODO: Remove children attachments.
}