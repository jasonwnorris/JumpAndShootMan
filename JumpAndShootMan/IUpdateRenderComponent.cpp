// IUpdateRenderComponent.cpp

// Project Includes
#include "IUpdateRenderComponent.hpp"

IUpdateRenderComponent::IUpdateRenderComponent(Entity* p_Owner) : IComponent(p_Owner)
{
	m_Name = "IUpdateRenderComponent";
}

IUpdateRenderComponent::~IUpdateRenderComponent()
{
}

bool IUpdateRenderComponent::CanUpdate() const
{
	return true;
}

bool IUpdateRenderComponent::CanRender() const
{
	return true;
}
