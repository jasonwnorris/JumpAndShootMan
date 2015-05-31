// IFixedUpdateComponent.cpp

// Project Includes
#include "IFixedUpdateComponent.hpp"

IFixedUpdateComponent::IFixedUpdateComponent(Entity* p_Owner) : IUpdateComponent(p_Owner)
{
	m_Name = "IFixedUpdateComponent";
	m_Accumulator = 0.0f;
	m_TimeStep = 1.0f / 60.0f;
	// TODO: Make TimeStep configurable.
}

IFixedUpdateComponent::~IFixedUpdateComponent()
{
}

bool IFixedUpdateComponent::Update(float p_DeltaTime)
{
	m_Accumulator += p_DeltaTime;

	// TODO: Remainder of Accumulator should be interpolated into next state.
	//       Currently causing some minor stuttering.
	while (m_Accumulator > m_TimeStep)
	{
		if (!FixedUpdate(m_TimeStep))
			return false;

		m_Accumulator -= m_TimeStep;
	}

	return true;
}