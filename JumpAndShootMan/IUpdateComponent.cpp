// IUpdateComponent.cpp

// Project Includes
#include "IUpdateComponent.hpp"

IUpdateComponent::IUpdateComponent(Entity* p_Owner) : IComponent(p_Owner)
{
	m_Name = "IUpdateComponent";
}

IUpdateComponent::~IUpdateComponent()
{
}

bool IUpdateComponent::CanUpdate() const
{
	return true;
}
