// PlatformerMotorComponent.hpp

#ifndef __PLATFORMERMOTORCOMPONENT_HPP__
#define __PLATFORMERMOTORCOMPONENT_HPP__

// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "IFixedUpdateComponent.hpp"
#include "TransformComponent.hpp"
#include "PhysicsComponent.hpp"
#include "MapColliderComponent.hpp"
#include "AudioSourceComponent.hpp"
#include "AnimationComponent.hpp"
// STL Includes
#include <string>

enum class TerrainState : int
{
	Ground,
	Air,
	Water,
};

enum class MotionState : int
{
	Idle,
	Walking,
	Stopping,
	Sprinting,
	Dashing,
	Jumping,
	Falling,
	FastFalling,
	LedgeGrabbing,
	WallClinging,
	Swimming,
};

class PlatformerMotorComponent : public IFixedUpdateComponent
{
	public:
		PlatformerMotorComponent(Entity* p_Owner);
		~PlatformerMotorComponent();

		bool IsFacingLeft() const;
		TerrainState GetTerrainState() const;
		std::string GetTerrainStateString() const;
		MotionState GetMotionState() const;
		std::string GetMotionStateString() const;

		void Move(const HGF::Vector2& p_Movement, bool p_IsNormalized = false);
		void SprintHeld(bool p_IsHeld);
		void Jump();
		void JumpHeld(bool p_IsHeld);
		void Fire();
		void Dash();
		void Fall();
		void Reset(const HGF::Vector2& p_Position = HGF::Vector2::Zero);

		virtual bool FixedUpdate(float p_DeltaTime) override;

	private:
		bool CanJump();
		bool CanDash();
		void SwitchMotionState(MotionState p_MotionState);
		void ClampFallSpeed();

		TransformComponent* m_Entity_TransformComponent;
		PhysicsComponent* m_Entity_PhysicsComponent;
		MapColliderComponent* m_Entity_MapColliderComponent;
		AnimationComponent* m_Entity_AnimationComponent;

		TerrainState m_TerrainState;
		MotionState m_MotionState;

		float m_TotalTime;

		float m_JumpGraceTime;
		float m_JumpGraceDuration;
		float m_MinimumJumpTime;
		float m_MaximumJumpTime;
		float m_TotalJumpTime;
		float m_JumpingAcceleration;
		
		bool m_HasSprintHeld;
		bool m_HasRequestedJump;
		bool m_HasExtendedJumpHeld;
		bool m_HasRequestedDash;
		bool m_HasRequestedFall;
		bool m_IsFacingLeft;

		float m_Gravity;
		float m_NormalizedHorizontalMovement;
		float m_NormalizedVerticalMovement;
		float m_MaximumMovementSpeedOnGround;
		float m_MaximumMovementSpeedInAir;
		float m_MaximumFallSpeed;
		float m_MaximumFastFallSpeed;

		float m_GroundSpeed;
		float m_GroundAcceleration;
		float m_GroundDeceleration;
		float m_GroundFriction;
		float m_AirAcceleration;

		float m_CollisionDistanceUp;
		float m_CollisionDistanceDown;
		float m_CollisionDistanceLeft;
		float m_CollisionDistanceRight;
		float m_LeaveGroundDistanceThreshold;

		int m_AllowedAirJumps;
		int m_AirJumpCount;
		int m_AllowedAirDashes;
		int m_AirDashCount;
};

#endif
