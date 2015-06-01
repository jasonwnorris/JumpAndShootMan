// InputComponent.cpp

// HGF Includes
#include <HGF\Keyboard.hpp>
// Project Includes
#include "InputComponent.hpp"
#include "ProjectileComponent.hpp"
#include "SpriteRenderComponent.hpp"
#include "Entity.hpp"

InputComponent::InputComponent(Entity* p_Owner) : IUpdateComponent(p_Owner)
{
	m_Name = "InputComponent";

	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();
	//m_Entity_PlatformerMotorComponent = m_Owner->GetComponent<PlatformerMotorComponent>();
	m_Entity_PlatformerMotorComponent = m_Owner->GetComponent<PlatformerMotorComponent>();
}

InputComponent::~InputComponent()
{
	m_Entity_PlatformerMotorComponent = nullptr;
}

bool InputComponent::Update(float p_DeltaTime)
{
	// TODO: Make keys configurable.
	HGF::Key resetKey = HGF::Key::Backspace;
	HGF::Key jumpKey = HGF::Key::Z;
	HGF::Key fireKey = HGF::Key::X;
	HGF::Key dashKey = HGF::Key::C;
	HGF::Key sprintKey = HGF::Key::LeftControl;
	HGF::Key upKey = HGF::Key::Up;
	HGF::Key downKey = HGF::Key::Down;
	HGF::Key leftKey = HGF::Key::Left;
	HGF::Key rightKey = HGF::Key::Right;

	// Reset Motor
	if (HGF::Keyboard::IsKeyPressed(resetKey))
	{
		m_Entity_PlatformerMotorComponent->Reset(HGF::Vector2(100.0f, 100.0f));
	}

	// Input: Jumping
	if (HGF::Keyboard::IsKeyPressed(jumpKey))
	{
		m_Entity_PlatformerMotorComponent->Jump();
	}
	m_Entity_PlatformerMotorComponent->JumpHeld(HGF::Keyboard::IsKeyDown(HGF::Key::Z));

	// Input: Firing
	if (HGF::Keyboard::IsKeyPressed(fireKey))
	{
		m_Entity_PlatformerMotorComponent->Fire();
	}

	// Input: Dashing
	if (HGF::Keyboard::IsKeyPressed(dashKey))
	{
		m_Entity_PlatformerMotorComponent->Dash();
	}

	// Input: Movement
	float dx = 0.0f;
	float dy = 0.0f;
	if (HGF::Keyboard::IsKeyDown(upKey))
	{
		dy += 1.0f;
	}
	if (HGF::Keyboard::IsKeyDown(downKey))
	{
		dy -= 1.0f;
	}
	if (HGF::Keyboard::IsKeyDown(leftKey))
	{
		dx -= 1.0f;
	}
	if (HGF::Keyboard::IsKeyDown(rightKey))
	{
		dx += 1.0f;
	}
	m_Entity_PlatformerMotorComponent->Move(HGF::Vector2::Up * dy + HGF::Vector2::Right * dx);
	m_Entity_PlatformerMotorComponent->SprintHeld(HGF::Keyboard::IsKeyDown(sprintKey));

	return true;
}