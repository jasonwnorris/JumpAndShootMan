// Player.hpp

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "AnimatedSprite.hpp"
#include "EntityManager.hpp"
#include "Raycast.hpp"
// STL Includes
#include <vector>

class Player : public Entity
{
	public:
		Player(EntityManager* pManager, World* pWorld);
		~Player();

		bool Load(const std::string& pFilename);
		void FireWeapon();

		void Update(float pDeltaTime) override;
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;
		void Render(SAGE::GeometryBatch& pGeometryBatch) override;

		HGF::Vector2 Position;
		HGF::Vector2 Velocity;
		HGF::Vector2 Acceleration;
		float Rotation;
		HGF::Vector2 Scale;
		float MovementSpeed;
		float JumpingSpeed;
		float Gravity;
		bool IsFacingLeft;
		bool IsGrounded;
		bool IsJumping;

	private:
		enum RayIndex
		{
			UP_CENTER,
			UP_LEFT,
			UP_RIGHT,
			DOWN_CENTER,
			DOWN_LEFT,
			DOWN_RIGHT,
			LEFT_CENTER,
			LEFT_TOP,
			LEFT_BOTTOM,
			RIGHT_CENTER,
			RIGHT_TOP,
			RIGHT_BOTTOM,
		};
		Ray mRays[12];
		RaycastData mRaycastDatas[12];

		AnimatedSprite mSprite;
		float mMinimumJumpTime;
		float mMaximumJumpTime;
		float mJumpTime;
};

#endif