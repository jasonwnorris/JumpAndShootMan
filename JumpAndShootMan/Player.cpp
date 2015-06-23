// Player.cpp

#include "Globals.hpp"
// HGF Includes
#include <HGF\Keyboard.hpp>
#include <HGF\MathUtil.hpp>
// Project Includes
#include "Player.hpp"
#include "World.hpp"

Player::Player(World* p_World) : Entity(p_World)
{
	// Requires: -
	m_TransformComponent = AddComponent<TransformComponent>();
	m_TransformComponent->SetPosition(HGF::Vector2(100.0f, 100.0f));
	m_TransformComponent->SetScale(HGF::Vector2::One * 0.5f);

	// Requires: TransformComponent
	m_PhysicsComponent = AddComponent<PhysicsComponent>();

	// Requires: TransformComponent, [TiledMap]
	m_MapColliderComponent = AddComponent<MapColliderComponent>();

	// Requires: -
	m_AnimationComponent = AddComponent<AnimationComponent>();
	m_AnimationComponent->Load("data/player.json");
	m_AnimationComponent->SetState("Movement", "Idle");

	// Requires: TransformComponent, AnimationComponent
	m_SpriteRenderComponent = AddComponent<SpriteRenderComponent>();
	m_SpriteRenderComponent->Load("data/img/player_sheet.png");

	// Requires: TransformComponent, PhysicsComponent, MapColliderComponent, AnimationComponent
	m_PlatformerMotorComponent = AddComponent<PlatformerMotorComponent>();

	// Requires: TransformComponent, PlatformerMotorComponent
	m_InputComponent = AddComponent<InputComponent>();
}

Player::~Player()
{
}
