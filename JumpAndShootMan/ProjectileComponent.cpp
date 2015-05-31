// ProjectileComponent.cpp

// Project Includes
#include "ProjectileComponent.hpp"
#include "Entity.hpp"

ProjectileComponent::ProjectileComponent(Entity* p_Owner) : IUpdateComponent(p_Owner)
{
	m_Name = "ProjectileComponent";

	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();

	// TODO: Make values configurable.
	m_LifeTime = 2.5f;
	m_Direction = HGF::Vector2::Left;
	m_Speed = 250.0f;
}

ProjectileComponent::~ProjectileComponent()
{
	m_Entity_TransformComponent = nullptr;
}

bool ProjectileComponent::Update(float p_DeltaTime)
{
	m_LifeTime -= p_DeltaTime;

	if (m_LifeTime < 0.0f)
	{
		m_Owner->Destroy();
	}
	else
	{
		m_Entity_TransformComponent->Translate(m_Direction * m_Speed * p_DeltaTime);
	}

	return true;
}