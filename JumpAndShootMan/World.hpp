// World.hpp

#ifndef __WORLD_HPP__
#define __WORLD_HPP__

// Project Includes
#include "TiledMap.hpp"
#include "IUpdateComponent.hpp"
#include "IRenderComponent.hpp"
#include "IUpdateRenderComponent.hpp"

class Entity;

class World
{
	public:
		World();
		~World();

		const TiledMap* GetMap() const;

		template<typename T> T* CreateEntity();
		void DestroyEntity(Entity* p_Entity);

		template<typename T> T* CreateComponent(Entity* p_Owner);
		void DestroyComponent(IComponent* p_Component);

		int Initialize();
		int Finalize();

		int Update(float p_DeltaTime);
		int Render(SAGE::SpriteBatch& p_SpriteBatch);
		int Render(SAGE::GeometryBatch& p_GeometryBatch);

	private:
		TiledMap m_TiledMap;
		std::vector<Entity*> m_Entities;
		std::vector<Entity*> m_EntitiesToAdd;
		std::vector<IComponent*> m_Components;
		std::vector<IComponent*> m_ComponentsToAdd;
};

template<typename T> T* World::CreateEntity()
{
	static_assert(std::is_base_of<Entity, T>::value, "Invalid Type");

	// Create the entity.
	T* entity = new T(this);

	// Queue to add to collection.
	m_EntitiesToAdd.push_back(entity);

	return entity;
}

template<typename T> T* World::CreateComponent(Entity* p_Owner)
{
	static_assert(std::is_base_of<IComponent, T>::value, "Invalid Type");

	// Create the component.
	T* component = new T(p_Owner);

	// Queue to add to collection.
	m_ComponentsToAdd.push_back(component);

	return component;
}

#endif
