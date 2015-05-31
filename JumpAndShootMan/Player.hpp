// Player.hpp

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "Entity.hpp"
#include "MapColliderComponent.hpp"
#include "PhysicsComponent.hpp"
#include "PlatformerMotorComponent.hpp"
#include "TransformComponent.hpp"
#include "AnimationComponent.hpp"
#include "SpriteRenderComponent.hpp"
#include "InputComponent.hpp"
// STL Includes
#include <vector>

class World;

class Player : public Entity
{
	public:
		Player(World* p_World);
		virtual ~Player();

	private:
		TransformComponent* m_TransformComponent;
		PhysicsComponent* m_PhysicsComponent;
		MapColliderComponent* m_MapColliderComponent;
		PlatformerMotorComponent* m_PlatformerMotorComponent;
		AnimationComponent* m_AnimationComponent;
		SpriteRenderComponent* m_SpriteRenderComponent;
		InputComponent* m_InputComponent;
};

#endif
