// PhysicsComponent.cpp

// Project Includes
#include "Entity.hpp"
#include "PhysicsComponent.hpp"

PhysicsComponent::PhysicsComponent(Entity* p_Owner) : IUpdateComponent(p_Owner)
{
	m_Name = "PhysicsComponent";

	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();

	m_CurrentVelocity = HGF::Vector2::Zero;
	m_PreviousVelocity = HGF::Vector2::Zero;
	m_CurrentAcceleration = HGF::Vector2::Zero;
	m_PreviousAcceleration = HGF::Vector2::Zero;
}

PhysicsComponent::~PhysicsComponent()
{
	m_Entity_TransformComponent = nullptr;
}

bool PhysicsComponent::Update(float p_DeltaTime)
{
	return true;
}

const HGF::Vector2& PhysicsComponent::GetVelocity() const
{
	return m_CurrentVelocity;
}

const HGF::Vector2& PhysicsComponent::GetPreviousVelocity() const
{
	return m_PreviousVelocity;
}

float PhysicsComponent::GetVelocityX() const
{
	return m_CurrentVelocity.X;
}

float PhysicsComponent::GetVelocityY() const
{
	return m_CurrentVelocity.Y;
}

void PhysicsComponent::SetVelocity(const HGF::Vector2& p_Velocity)
{
	SetVelocityX(p_Velocity.X);
	SetVelocityY(p_Velocity.Y);
}

void PhysicsComponent::SetVelocity(float p_VelocityX, float p_VelocityY)
{
	SetVelocityX(p_VelocityX);
	SetVelocityY(p_VelocityY);
}

void PhysicsComponent::SetVelocityX(float p_VelocityX)
{
	m_PreviousVelocity.X = m_CurrentVelocity.X;
	m_CurrentVelocity.X = p_VelocityX;
}

void PhysicsComponent::SetVelocityY(float p_VelocityY)
{
	m_PreviousVelocity.Y = m_CurrentVelocity.Y;
	m_CurrentVelocity.Y = p_VelocityY;
}

void PhysicsComponent::ResetVelocity()
{
	ResetVelocity(0.0f, 0.0f);
}

void PhysicsComponent::ResetVelocity(const HGF::Vector2& p_Velocity)
{
	ResetVelocity(p_Velocity.X, p_Velocity.Y);
}

void PhysicsComponent::ResetVelocity(float p_VelocityX, float p_VelocityY)
{
	m_PreviousVelocity.X = p_VelocityX;
	m_PreviousVelocity.Y = p_VelocityY;
	m_CurrentVelocity.X = p_VelocityX;
	m_CurrentVelocity.Y = p_VelocityY;
}

const HGF::Vector2& PhysicsComponent::GetAcceleration() const
{
	return m_CurrentAcceleration;
}

const HGF::Vector2& PhysicsComponent::GetPreviousAcceleration() const
{
	return m_PreviousAcceleration;
}

float PhysicsComponent::GetAccelerationX() const
{
	return m_CurrentAcceleration.X;
}

float PhysicsComponent::GetAccelerationY() const
{
	return m_CurrentAcceleration.Y;
}

void PhysicsComponent::SetAcceleration(const HGF::Vector2& p_Acceleration)
{
	SetAccelerationX(p_Acceleration.X);
	SetAccelerationY(p_Acceleration.Y);
}

void PhysicsComponent::SetAcceleration(float p_AccelerationX, float p_AccelerationY)
{
	SetAccelerationX(p_AccelerationX);
	SetAccelerationY(p_AccelerationY);
}

void PhysicsComponent::SetAccelerationX(float p_AccelerationX)
{
	m_PreviousAcceleration.X = m_CurrentAcceleration.X;
	m_CurrentAcceleration.X = p_AccelerationX;
}

void PhysicsComponent::SetAccelerationY(float p_AccelerationY)
{
	m_PreviousAcceleration.Y = m_CurrentAcceleration.Y;
	m_CurrentAcceleration.Y = p_AccelerationY;
}

void PhysicsComponent::ResetAcceleration()
{
	ResetAcceleration(0.0f, 0.0f);
}

void PhysicsComponent::ResetAcceleration(const HGF::Vector2& p_Acceleration)
{
	ResetAcceleration(p_Acceleration.X, p_Acceleration.Y);
}

void PhysicsComponent::ResetAcceleration(float p_AccelerationX, float p_AccelerationY)
{
	m_PreviousAcceleration.X = p_AccelerationX;
	m_PreviousAcceleration.Y = p_AccelerationY;
	m_CurrentAcceleration.X = p_AccelerationX;
	m_CurrentAcceleration.Y = p_AccelerationY;
}
