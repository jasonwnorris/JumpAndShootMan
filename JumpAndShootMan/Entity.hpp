// Entity.hpp

#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Timer.hpp>
// Project Includes
#include "IComponent.hpp"
#include "IDestroyable.hpp"
// STL Includes
#include <map>
#include <typeindex>

class World;

class Entity : public IDestroyable
{
	friend class World;

	public:
		World* GetWorld() const;

		template<typename T> T* AddComponent();
		template<typename T> void RemoveComponent();
		template<typename T> T* GetComponent();

		virtual void Destroy() override;

	protected:
		Entity(World* p_World);
		virtual ~Entity();

		World* m_World;
		Entity* m_Parent;
		std::vector<Entity*> m_Children;
		// NOTE: It's assumed that std::type_index is unique for each entity.
		std::map<std::type_index, IComponent*> m_Components;
};

template<typename T> T* Entity::AddComponent()
{
	static_assert(std::is_base_of<IComponent, T>::value, "Invalid Component Type");

	// Acquire class type.
	std::type_index index(typeid(T));

	// Create the component.
	T* component = m_World->CreateComponent<T>(this);

	// Store against type.
	m_Components[index] = component;

	return component;
}

template<typename T> void Entity::RemoveComponent()
{
	static_assert(std::is_base_of<IComponent, T>::value, "Invalid Component Type");

	// Acquire class type.
	std::type_index index(typeid(T));

	// Check if exists.
	std::map<std::type_index, IComponent*>::iterator iter = m_Components.find(index);
	if (iter != m_Components.end())
	{
		// Mark for removal.
		m_Components[index]->Destroy();

		// Remove from collection.
		m_Components.erase(iter);
	}
}

template<typename T> T* Entity::GetComponent()
{
	static_assert(std::is_base_of<IComponent, T>::value, "Invalid Component Type");

	// Acquire class type.
	std::type_index index(typeid(T));

	// Search for component.
	if (m_Components.find(index) != m_Components.end())
	{
		return static_cast<T*>(m_Components[index]);
	}

	return nullptr;
}

#endif
