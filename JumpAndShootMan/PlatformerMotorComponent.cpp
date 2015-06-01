// PlatformerMotorComponent.cpp

// HGF Includes
#include <HGF\MathUtil.hpp>
// Project Includes
#include "Entity.hpp"
#include "World.hpp"
#include "PlatformerMotorComponent.hpp"
#include "ProjectileComponent.hpp"
#include "SpriteRenderComponent.hpp"

PlatformerMotorComponent::PlatformerMotorComponent(Entity* p_Owner) : IFixedUpdateComponent(p_Owner)
{
	m_Name = "PlatformerMotorComponent";

	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();
	m_Entity_PhysicsComponent = m_Owner->GetComponent<PhysicsComponent>();
	m_Entity_MapColliderComponent = m_Owner->GetComponent<MapColliderComponent>();
	
	m_TerrainState = TerrainState::Air;
	m_MotionState = MotionState::Falling;

	m_TotalTime = 0.0f;

	m_JumpGraceTime = 0.0f;
	m_JumpGraceDuration = 0.5f;
	m_MinimumJumpTime = 0.1f;
	m_MaximumJumpTime = 0.25f;
	m_TotalJumpTime = 0.0f;
	m_JumpingAcceleration = 100.0f;

	m_HasSprintHeld = false;
	m_HasRequestedJump = false;
	m_HasExtendedJumpHeld = false;
	m_HasRequestedDash = false;
	m_HasRequestedFall = false;
	m_IsFacingLeft = false;

	m_Gravity = 25.0f;
	m_NormalizedHorizontalMovement = 0.0f;
	m_NormalizedVerticalMovement = 0.0f;
	m_MaximumMovementSpeedOnGround = 6.5f;
	m_MaximumMovementSpeedInAir = 7.5f;
	m_MaximumFallSpeed = 7.5f;
	m_MaximumFastFallSpeed = 12.5f;

	m_GroundSpeed = 0.0f;
	m_GroundAcceleration = 5.0f;
	m_GroundDeceleration = 8.5f;
	m_GroundFriction = 6.75f;
	m_AirAcceleration = 5.0f;

	m_CollisionDistanceUp = 35.0f;
	m_CollisionDistanceDown = 35.0f;
	m_CollisionDistanceLeft = 25.0f;
	m_CollisionDistanceRight = 25.0f;
	m_LeaveGroundDistanceThreshold = 40.0f;

	m_AllowedAirJumps = 1;
	m_AirJumpCount = 0;
	m_AllowedAirDashes = 1;
	m_AirDashCount = 0;
}

PlatformerMotorComponent::~PlatformerMotorComponent()
{
	m_Entity_TransformComponent = nullptr;
	m_Entity_PhysicsComponent = nullptr;
	m_Entity_MapColliderComponent = nullptr;
}

bool PlatformerMotorComponent::IsFacingLeft() const
{
	return m_IsFacingLeft;
}
TerrainState PlatformerMotorComponent::GetTerrainState() const
{
	return m_TerrainState;
}

std::string PlatformerMotorComponent::GetTerrainStateString() const
{
	if (m_TerrainState == TerrainState::Ground)
		return "Ground";
	else if (m_TerrainState == TerrainState::Air)
		return "Air";
	else if (m_TerrainState == TerrainState::Water)
		return "Water";

	return "UNKNOWN";
}

MotionState PlatformerMotorComponent::GetMotionState() const
{
	return m_MotionState;
}

std::string PlatformerMotorComponent::GetMotionStateString() const
{
	if (m_MotionState == MotionState::Idle)
		return "Idle";
	else if (m_MotionState == MotionState::Walking)
		return "Walking";
	else if (m_MotionState == MotionState::Stopping)
		return "Stopping";
	else if (m_MotionState == MotionState::Sprinting)
		return "Sprinting";
	else if (m_MotionState == MotionState::Dashing)
		return "Dashing";
	else if (m_MotionState == MotionState::Jumping)
		return "Jumping";
	else if (m_MotionState == MotionState::Falling)
		return "Falling";
	else if (m_MotionState == MotionState::FastFalling)
		return "FastFalling";
	else if (m_MotionState == MotionState::LedgeGrabbing)
		return "LedgeGrabbing";
	else if (m_MotionState == MotionState::WallClinging)
		return "WallClinging";
	
	return "UNKNOWN";
}

void PlatformerMotorComponent::Move(const HGF::Vector2& p_Movement, bool p_IsNormalized)
{
	// Accept values if normalized.
	if (p_IsNormalized)
	{
		m_NormalizedHorizontalMovement = p_Movement.X;
		m_NormalizedVerticalMovement = p_Movement.Y;
	}
	else
	{
		// Otherwise do normalization.
		float length = p_Movement.Length();
		if (length != 0.0f)
		{
			m_NormalizedHorizontalMovement = p_Movement.X / length;
			m_NormalizedVerticalMovement = p_Movement.Y / length;
		}
		else
		{
			m_NormalizedHorizontalMovement = 0.0f;
			m_NormalizedVerticalMovement = 0.0f;
		}
	}

	// Facing Direction
	if (p_Movement.X > 0.0f)
	{
		m_IsFacingLeft = false;
	}
	else if (p_Movement.X < 0.0f)
	{
		m_IsFacingLeft = true;
	}
}

void PlatformerMotorComponent::SprintHeld(bool p_IsHeld)
{
	m_HasSprintHeld = p_IsHeld;
}

void PlatformerMotorComponent::Jump()
{
	m_HasRequestedJump = true;
	m_HasExtendedJumpHeld = true;
}

void PlatformerMotorComponent::JumpHeld(bool p_IsHeld)
{
	m_HasExtendedJumpHeld = p_IsHeld;
}

void PlatformerMotorComponent::Fire()
{
	Entity* e = m_Owner->GetWorld()->CreateEntity<Entity>();

	TransformComponent* tc = e->AddComponent<TransformComponent>();
	tc->SetPosition(m_Entity_TransformComponent->GetPosition());

	ProjectileComponent* pc = e->AddComponent<ProjectileComponent>();
	pc->SetLifeTime(2.5f);
	pc->SetDirection(m_IsFacingLeft ? HGF::Vector2::Left : HGF::Vector2::Right);
	pc->SetSpeed(500.0f);

	SpriteRenderComponent* src = e->AddComponent<SpriteRenderComponent>();
	src->Load("data/img/projectile.png");
}

void PlatformerMotorComponent::Dash()
{
	m_HasRequestedDash = true;
}

void PlatformerMotorComponent::Fall()
{
	m_HasRequestedFall = true;
}

void PlatformerMotorComponent::Reset(const HGF::Vector2& p_Position)
{
	m_Entity_TransformComponent->ResetPosition(p_Position);
	m_Entity_PhysicsComponent->ResetVelocity();
	m_Entity_PhysicsComponent->ResetAcceleration();
}

bool PlatformerMotorComponent::FixedUpdate(float p_DeltaTime)
{
	// Grab map collision distances.
	float upDistance = m_Entity_MapColliderComponent->DistanceUpward();
	float downDistance = m_Entity_MapColliderComponent->DistanceDownward();
	float leftDistance = m_Entity_MapColliderComponent->DistanceLeftward();
	float rightDistance = m_Entity_MapColliderComponent->DistanceRightward();

	// Check Jump
	if (m_HasRequestedJump && CanJump())
	{
		if (m_TerrainState == TerrainState::Air)
		{
			++m_AirJumpCount;
		}

		m_TerrainState = TerrainState::Air;
		m_MotionState = MotionState::Jumping;
		m_TotalJumpTime = 0.0f;
		m_Entity_PhysicsComponent->SetVelocityY(0.0f);
	}
	m_HasRequestedJump = false;

	// Check Dash
	if (m_HasRequestedDash && CanDash())
	{
	}

	// Check Fall
	if (m_HasRequestedFall)
	{
		if (m_MotionState == MotionState::Falling)
		{
			m_MotionState = MotionState::FastFalling;
		}
		else if (m_MotionState == MotionState::Jumping || m_MotionState == MotionState::LedgeGrabbing || m_MotionState == MotionState::WallClinging)
		{
			m_MotionState = MotionState::Falling;
		}
	}

	// Motion
	if (m_TerrainState == TerrainState::Ground)
	{
		// Apply friction if not trying to move.
		if (m_NormalizedHorizontalMovement == 0.0f)
		{
			if (HGF::MathUtil::Abs(m_Entity_PhysicsComponent->GetVelocityX()) < m_GroundFriction * p_DeltaTime)
			{
				// Stop if under friction threshold.
				m_Entity_PhysicsComponent->SetVelocityX(0.0f);
				m_MotionState = MotionState::Idle;
			}
			else
			{
				// Apply friction.
				m_Entity_PhysicsComponent->SetVelocityX(m_Entity_PhysicsComponent->GetVelocityX() - m_GroundFriction * HGF::MathUtil::SignOf(m_Entity_PhysicsComponent->GetVelocityX()) * p_DeltaTime);
				m_MotionState = MotionState::Stopping;
			}
		}
		else
		{
			// Accelerate if not in motion or continuing the same direction.
			// Otherwise decelerate into opposite direction.
			if ((m_Entity_PhysicsComponent->GetVelocityX() == 0.0f) ||
				(m_NormalizedHorizontalMovement > 0.0f && m_Entity_PhysicsComponent->GetVelocityX() > 0.0f) ||
				(m_NormalizedHorizontalMovement < 0.0f && m_Entity_PhysicsComponent->GetVelocityX() < 0.0f))
			{
				if (HGF::MathUtil::Abs(m_Entity_PhysicsComponent->GetVelocityX()) > m_MaximumMovementSpeedOnGround)
				{
					// Enforce speed limit.
					m_Entity_PhysicsComponent->SetVelocityX(m_MaximumMovementSpeedOnGround * HGF::MathUtil::SignOf(m_Entity_PhysicsComponent->GetVelocityX()));
					m_MotionState = MotionState::Walking;
				}
				else
				{
					// Apply movement.
					m_Entity_PhysicsComponent->SetVelocityX(m_Entity_PhysicsComponent->GetVelocityX() + m_GroundAcceleration * m_NormalizedHorizontalMovement * p_DeltaTime);
					m_MotionState = MotionState::Walking;
				}
			}
			else
			{
				// Decelerate.
				m_Entity_PhysicsComponent->SetVelocityX(m_Entity_PhysicsComponent->GetVelocityX() + m_GroundDeceleration * m_NormalizedHorizontalMovement * p_DeltaTime);
				m_MotionState = MotionState::Stopping;
			}
		}
	}
	else if (m_TerrainState == TerrainState::Air)
	{
		// Vertical movement.
		if (m_MotionState == MotionState::Jumping)
		{
			// Lift from jumping.
			m_Entity_PhysicsComponent->SetVelocityY(m_Entity_PhysicsComponent->GetVelocityY() -
				                                    m_JumpingAcceleration * sqrtf(m_MaximumJumpTime - fminf(m_TotalJumpTime, m_MaximumJumpTime)) * p_DeltaTime);

			// Increment jumping time.
			m_TotalJumpTime += p_DeltaTime;
			if (m_TotalJumpTime > m_MaximumJumpTime || (m_TotalJumpTime > m_MinimumJumpTime && !m_HasExtendedJumpHeld))
			{
				m_MotionState = MotionState::Falling;
			}
		}
		else if (m_MotionState == MotionState::Falling)
		{
			if (HGF::MathUtil::Abs(m_Entity_PhysicsComponent->GetVelocityY() + m_Gravity * p_DeltaTime) > m_MaximumFallSpeed)
			{
				// Enforce terminal velocity.
				m_Entity_PhysicsComponent->SetVelocityY(m_MaximumFallSpeed * HGF::MathUtil::SignOf(m_Entity_PhysicsComponent->GetVelocityY()));
			}
			else
			{
				// Fall from gravity.
				m_Entity_PhysicsComponent->SetVelocityY(m_Entity_PhysicsComponent->GetVelocityY() + m_Gravity * p_DeltaTime);
			}
		}
		else if (m_MotionState == MotionState::FastFalling)
		{
		}

		// Horizontal movement.
		if (HGF::MathUtil::Abs(m_Entity_PhysicsComponent->GetVelocityX()) > m_MaximumMovementSpeedInAir)
		{
			// Enforce speed limit.
			m_Entity_PhysicsComponent->SetVelocityX(m_MaximumMovementSpeedInAir * HGF::MathUtil::SignOf(m_Entity_PhysicsComponent->GetVelocityX()));
		}
		else
		{
			// TODO: Apply air drag formula?
			// Apply movement.
			m_Entity_PhysicsComponent->SetVelocityX(m_Entity_PhysicsComponent->GetVelocityX() + m_AirAcceleration * m_NormalizedHorizontalMovement * p_DeltaTime);
		}
	}
	else if (m_TerrainState == TerrainState::Water)
	{
	}

	// Apply velocity to position.
	m_Entity_TransformComponent->Translate(m_Entity_PhysicsComponent->GetVelocity());

	// Collision: Down
	if (m_TerrainState == TerrainState::Air)
	{
		if (downDistance < m_CollisionDistanceDown && m_Entity_PhysicsComponent->GetVelocityY() >= 0.0f)
		{
			m_TerrainState = TerrainState::Ground;
			m_MotionState = MotionState::Idle;
			m_AirJumpCount = 0;
			m_Entity_TransformComponent->TranslateY(downDistance - m_CollisionDistanceDown);
			m_Entity_PhysicsComponent->SetVelocityY(0.0f);
		}
	}
	else if (m_TerrainState == TerrainState::Ground)
	{
		if (downDistance > m_LeaveGroundDistanceThreshold)
		{
			m_TerrainState = TerrainState::Air;
			m_MotionState = MotionState::Falling;
		}
		else
		{
			m_Entity_TransformComponent->TranslateY(downDistance - m_CollisionDistanceDown);
		}
	}

	// Collision: Left
	if (leftDistance < m_CollisionDistanceLeft)
	{
		m_Entity_TransformComponent->TranslateX(m_CollisionDistanceLeft - leftDistance);
		m_Entity_PhysicsComponent->SetVelocityX(0.0f);

		if (m_MotionState == MotionState::Falling)
		{
			if (m_NormalizedHorizontalMovement < -0.5f && m_Entity_PhysicsComponent->GetVelocityY() > 0.0f)
			{
				m_MotionState = MotionState::WallClinging;
				m_Entity_PhysicsComponent->SetVelocityY(0.0f);
			}
		}
		else if (m_MotionState == MotionState::WallClinging)
		{
			if (m_NormalizedHorizontalMovement > -0.5f)
			{
				m_MotionState = MotionState::Falling;
			}
		}
	}

	// Collision: Right
	if (rightDistance < m_CollisionDistanceRight)
	{
		m_Entity_TransformComponent->TranslateX(rightDistance - m_CollisionDistanceRight);
		m_Entity_PhysicsComponent->SetVelocityX(0.0f);

		if (m_MotionState == MotionState::Falling)
		{
			if (m_NormalizedHorizontalMovement > 0.5f && m_Entity_PhysicsComponent->GetVelocityY() > 0.0f)
			{
				m_MotionState = MotionState::WallClinging;
				m_Entity_PhysicsComponent->SetVelocityY(0.0f);
			}
		}
		else if (m_MotionState == MotionState::WallClinging)
		{
			if (m_NormalizedHorizontalMovement < 0.5f)
			{
				m_MotionState = MotionState::Falling;
			}
		}
	}

	// Collision: Up
	if (upDistance < m_CollisionDistanceUp)
	{
		m_Entity_TransformComponent->TranslateY(m_CollisionDistanceUp - upDistance);
		m_Entity_PhysicsComponent->SetVelocityY(0.0f);
	}

	// Timers
	m_TotalTime += p_DeltaTime;

	return true;
}

bool PlatformerMotorComponent::CanJump()
{
	return m_TerrainState == TerrainState::Ground || m_TotalTime < m_JumpGraceTime || m_AirJumpCount < m_AllowedAirJumps;
}

bool PlatformerMotorComponent::CanDash()
{
	return m_TerrainState == TerrainState::Ground || m_AirDashCount < m_AllowedAirDashes;
}

void PlatformerMotorComponent::SwitchMotionState(MotionState p_MotionState)
{
	m_MotionState = p_MotionState;
}

void PlatformerMotorComponent::ClampFallSpeed()
{
	if (m_MotionState == MotionState::Falling && m_Entity_PhysicsComponent->GetVelocityY() > m_MaximumFallSpeed)
	{
		m_Entity_PhysicsComponent->SetVelocityY(m_MaximumFallSpeed);
	}
	else if (m_MotionState == MotionState::FastFalling && m_Entity_PhysicsComponent->GetVelocityY() > m_MaximumFastFallSpeed)
	{
		m_Entity_PhysicsComponent->SetVelocityY(m_MaximumFastFallSpeed);
	}
}