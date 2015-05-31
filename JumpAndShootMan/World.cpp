// World.cpp

// Project Includes
#include "Globals.hpp"
#include "Entity.hpp"
#include "World.hpp"
// STL Includes
#include <algorithm>

World::World()
{
}

World::~World()
{
	// Delete entities.
	std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity* entity) { delete entity; });
	m_Entities.clear();

	// Delete queued entities.
	std::for_each(m_EntitiesToAdd.begin(), m_EntitiesToAdd.end(), [&](Entity* entity) { delete entity; });
	m_EntitiesToAdd.clear();

	// Delete components.
	std::for_each(m_Components.begin(), m_Components.end(), [&](IComponent* component) { delete component; });
	m_Components.clear();

	// Delete queued components.
	std::for_each(m_ComponentsToAdd.begin(), m_ComponentsToAdd.end(), [&](IComponent* component) { delete component; });
	m_ComponentsToAdd.clear();
}

const TiledMap* World::GetMap() const
{
	return &m_TiledMap;
}

void World::DestroyEntity(Entity* p_Entity)
{
	p_Entity->Destroy();
}

void World::DestroyComponent(IComponent* p_Component)
{
	p_Component->Destroy();
}

int World::Initialize()
{
	// Load the map.
	if (!m_TiledMap.Load("data/maps/first.json"))
	{
		return -1;
	}

	return 0;
}

int World::Finalize()
{
	return 0;
}

int World::Update(float p_DeltaTime)
{
	m_TiledMap.Update(p_DeltaTime);

	// Remove entities marked for removal.
	m_Entities.erase(std::remove_if(m_Entities.begin(), m_Entities.end(), [](Entity* entity) {
		if (!entity->IsAlive())
		{
			delete entity;

			return true;
		}
		else
		{
			return false;
		}
	}), m_Entities.end());

	// Push in new entities.
	std::for_each(m_EntitiesToAdd.begin(), m_EntitiesToAdd.end(), [&](Entity* entity) { m_Entities.push_back(entity); });
	m_EntitiesToAdd.clear();

	// Remove components marked for removal.
	m_Components.erase(std::remove_if(m_Components.begin(), m_Components.end(), [](IComponent* component) {
		if (!component->IsAlive())
		{
			delete component;

			return true;
		}
		else
		{
			return false;
		}
	}), m_Components.end());

	// Push in new components.
	std::for_each(m_ComponentsToAdd.begin(), m_ComponentsToAdd.end(), [&](IComponent* component) { m_Components.push_back(component); });
	m_ComponentsToAdd.clear();

	// Update active components.
	std::for_each(m_Components.begin(), m_Components.end(), [&](IComponent* component) { 
		if (component->CanUpdate())
		{
			// TODO: Eww, no.
			IUpdateComponent* c = dynamic_cast<IUpdateComponent*>(component);
			if (c != nullptr)
			{
				c->Update(p_DeltaTime);
			}
			else
			{
				IUpdateRenderComponent* cc = dynamic_cast<IUpdateRenderComponent*>(component);
				if (cc != nullptr)
				{
					cc->Update(p_DeltaTime);
				}
			}
		}
	});

	return 0;
}

int World::Render(SAGE::SpriteBatch& p_SpriteBatch)
{
	// Render map.
	m_TiledMap.Render(p_SpriteBatch);

	// Render components.
	std::for_each(m_Components.begin(), m_Components.end(), [&](IComponent* component) {
		if (component->CanRender())
		{
			// TODO: Eww, no.
			IRenderComponent* c = dynamic_cast<IRenderComponent*>(component);
			if (c != nullptr)
			{
				c->Render(p_SpriteBatch);
			}
			else
			{
				IUpdateRenderComponent* cc = dynamic_cast<IUpdateRenderComponent*>(component);
				if (cc != nullptr)
				{
					cc->Render(p_SpriteBatch);
				}
			}
		}
	});

	return 0;
}

int World::Render(SAGE::GeometryBatch& p_GeometryBatch)
{
	// Render map.
	m_TiledMap.Render(p_GeometryBatch);

	// Render components.
	std::for_each(m_Components.begin(), m_Components.end(), [&](IComponent* component) {
		if (component->CanRender())
		{
			// TODO: Eww, no.
			IRenderComponent* c = dynamic_cast<IRenderComponent*>(component);
			if (c != nullptr)
			{
				c->Render(p_GeometryBatch);
			}
			else
			{
				IUpdateRenderComponent* cc = dynamic_cast<IUpdateRenderComponent*>(component);
				if (cc != nullptr)
				{
					cc->Render(p_GeometryBatch);
				}
			}
		}
	});

	return 0;
}
