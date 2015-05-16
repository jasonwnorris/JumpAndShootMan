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

		void Move(const HGF::Vector2& p_Movement);
		void Jump();
		void Fire();
		void Dash();
		void Fall();

		HGF::Vector2 Position;
		HGF::Vector2 Velocity;
		HGF::Vector2 Acceleration;
		float Rotation;
		HGF::Vector2 Scale;
		float MovementSpeed;
		float JumpingSpeed;
		float Gravity;
		bool IsFacingLeft;
		bool IsOnGround;
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

		enum class MotorState
		{
			Grounded,
			Jumping,
			Falling,
			FastFalling,
			WallSliding,
			WallClinging,
			LedgeGrabbing,
			Dashing,
			Frozen,
		} m_MotorState;

		enum class CollisionSurface
		{
			None,
			Floor,
			Ceiling,
			LeftWall,
			RightWall,
		} m_CollisionSurface;

		struct DashState
		{
			bool IsPressed;
			bool IsForced;
			bool IsWithDirection;
			HGF::Vector2 Direction;
			float CooldownTimer;
			float TimeDashed;
			float GravityEnabledTimer;
			float DistanceCalculated;
			float DistanceDashed;
		} m_DashState;

		struct JumpState
		{
			public:
				enum JumpType
				{
					None,
					Normal,
					RightWall,
					LeftWall,
					Ledge
				};

				JumpType GetLastValidJump()
				{
					return m_LastJumpType;
				}

				void SetLastValidJump(JumpType p_JumpType)
				{
					if (p_JumpType != JumpType::None)
					{
						IsJumpTypeChanged = true;
					}
					else
					{
						JumpGraceTime = 0.0f;
					}

					m_LastJumpType = p_JumpType;
				}

				bool IsPressed;
				bool IsHeld;
				bool IsJumpTypeChanged;
				bool IsForced;
				bool IsIgnoreGravity;
				int AirJumpCount;
				float AllowExtraDuration;
				float JumpGraceTime;
				float TimePressed;
				float Height;

			private:
				JumpType m_LastJumpType;

		} m_JumpState;

		struct WallState
		{
			bool CanHangAgain;
			float WallClingTime;
			float LedgeGrabTime;
		} m_WallState;

		void Update_Input(float pDeltaTime);
		void Update_FixedUpdateA(float pDeltaTime);
		void Update_AfterTickA(float pDeltaTime);
		void Update_FixedUpdateB(float pDeltaTime);
		void Update_AfterTickB(float pDeltaTime);

		bool HasFlag(CollisionSurface p_CollisionSurface) const;
		bool IsGrounded() const;
		bool IsAirborne() const;
		bool IsPressingIntoLeftWall() const;
		bool IsPressingIntoRightWall() const;
		bool IsAtLedge() const;
		void SetFacing();
		void ClampFallSpeed();
		float CalculateRequiredSpeed(float p_Height) const;
		void ApplyMovement(float p_DeltaTime);
		void Accelerate(float p_Acceleration, float p_Limit, float p_DeltaTime);
		void Decelerate(float p_Deceleration, float p_Limit, float p_DeltaTime);
		void StartDash(float p_DeltaTime);
		void HandleDash(float p_DeltaTime);
		void HandlePreWallInteraction();
		void HandlePostWallInteraction();

		float m_MaxGroundSpeed;
		float m_MaxAirSpeed;
		float m_TimeToMaxGroundSpeed;
		float m_TimeToMaxAirSpeed;
		float m_DistanceForStopOnGround;
		float m_DistanceForStopInAir;
		bool m_CanChangeDirectionInAir;
		float m_MaxFallSpeed;
		float m_MaxFastFallSpeed;
		float m_FastFallGravityMultiplier;
		float m_MinJumpHeight;
		float m_MaxJumpHeight;
		float m_JumpGraceTime;
		int m_AirJumpCount;
		bool m_CanWallJump;
		float m_WallJumpSpeedMultiplier;
		bool m_CanWallCling;
		float m_WallClingDuration;
		bool m_CanWallSlide;
		float m_WallSlideSpeedMultiplier;
		bool m_CanGrabLedge;
		float m_LedgeGrabJumpSpeedMultiplier;
		float m_LedgeGrabDuration;
		float m_LedgeGrabCheckDistance;
		bool m_CanDash;
		float m_DashCooldown;
		float m_DashDistance;
		float m_DashDuration;
		float m_DashEndDelay;

		float m_NormalizedHorizontalMovement;

		HGF::Vector2 m_PreviousPosition;

		// TODO: Convert to new versions.
		Ray mRays[12];
		RaycastData mRaycastDatas[12];
		AnimatedSprite mSprite;
		float mMinimumJumpTime;
		float mMaximumJumpTime;
		float mJumpTime;
};

#endif