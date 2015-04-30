// Player.hpp

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Texture.hpp>
#include <HGF\Vector2.hpp>
// Project Includes
#include "EntityManager.hpp"
#include "ITiledMapCollider.hpp"
#include "Raycast.hpp"
// STL Includes
#include <vector>

class Player : public Entity, public ITiledMapCollider
{
	public:
		Player(EntityManager* pManager, World* pWorld);
		~Player();

		bool Load(const std::string& pFilename);
		void Fire();

		void OnCollision(Direction pDirection, float pDistance) override;
		void OnEmpty(Direction pDirection, float pDistance) override;

		void Update(float pDeltaTime) override;
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;
		void Render(SAGE::GeometryBatch& pGeometryBatch) override;

		HGF::Vector2 Position;
		HGF::Vector2 Velocity;
		HGF::Vector2 Acceleration;
		bool IsFacingLeft;
		float MovementSpeed;
		float JumpingSpeed;
		float Gravity;
		HGF::Rectangle Source;
		HGF::Vector2 Scale;
		HGF::Vector2 Origin;
		HGF::Vector2 Dimensions;
		bool IsGrounded;
		bool IsJumping;
		bool IsDebugFly;

	private:
		HGF::Texture mTexture;
		float mMinimumJumpTime;
		float mMaximumJumpTime;
		float mJumpTime;
};

#endif