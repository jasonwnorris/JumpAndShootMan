// ProjectileComponent.cpp

// Project Includes
#include "ProjectileComponent.hpp"
#include "Entity.hpp"

ProjectileComponent::ProjectileComponent(Entity* p_Owner) : IUpdateComponent(p_Owner)
{
	m_Name = "ProjectileComponent";

	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();
}

ProjectileComponent::~ProjectileComponent()
{
	m_Entity_TransformComponent = nullptr;
}

void ProjectileComponent::SetLifeTime(float p_LifeTime)
{
	m_LifeTime = p_LifeTime;
}

void ProjectileComponent::SetDirection(const HGF::Vector2& p_Direction)
{
	m_Direction = p_Direction;
}

void ProjectileComponent::SetSpeed(float p_Speed)
{
	m_Speed = p_Speed;
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