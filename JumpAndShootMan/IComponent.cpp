// IComponent.cpp

// Project Includes
#include "IComponent.hpp"

IComponent::IComponent(Entity* p_Owner)
{
	m_Owner = p_Owner;
	m_Name = "IComponent";
}

IComponent::~IComponent()
{
	m_Owner = nullptr;
}

Entity* IComponent::GetOwner() const
{
	return m_Owner;
}

const std::string& IComponent::GetName() const
{
	return m_Name;
}

bool IComponent::CanUpdate() const
{
	return false;
}

bool IComponent::CanRender() const
{
	return false;
}
