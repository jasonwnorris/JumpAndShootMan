// IRenderComponent.cpp

// Project Includes
#include "IRenderComponent.hpp"

IRenderComponent::IRenderComponent(Entity* p_Owner) : IComponent(p_Owner)
{
	m_Name = "IRenderComponent";
}

IRenderComponent::~IRenderComponent()
{
}

bool IRenderComponent::CanRender() const
{
	return true;
}
