// Player.cpp

// HGF Includes
#include <HGF\Keyboard.hpp>
#include <HGF\MathUtil.hpp>
// Project Includes
#include "Globals.hpp"
#include "Player.hpp"
#include "DirectionalProjectile.hpp"
#include "World.hpp"

// TODO: Make as a function pointer.
// NOTE: Linear interpolation.
static float _dashFunction(float p_Start, float p_End, float p_Value)
{
	return HGF::MathUtil::Lerp(p_Start, p_End, p_Value);
}

Player::Player(EntityManager* pManager, World* pWorld) : Entity(pManager, pWorld)
{
	Position = HGF::Vector2(100.0f, 100.0f);
	Velocity = HGF::Vector2::Zero;
	Acceleration = HGF::Vector2::Zero;
	Rotation = 0.0f;
	Scale = HGF::Vector2::One * 0.5f;
	MovementSpeed = 500.0f;
	JumpingSpeed = 150.0f;
	Gravity = 20.0f;
	IsFacingLeft = false;
	IsOnGround = false;
	IsJumping = false;

	mMinimumJumpTime = 0.05f;
	mMaximumJumpTime = 0.75f;
	mJumpTime = 0.0f;

	float upSpread = 14.0f;
	float downSpreed = 18.0f;
	float sideSpread = 22.5f;

	mRays[UP_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	mRays[UP_CENTER].Direction = HGF::Vector2::Up;
	mRays[UP_CENTER].Mask = 2;
	mRays[UP_LEFT].Position = HGF::Vector2(-upSpread, 0.0f);
	mRays[UP_LEFT].Direction = HGF::Vector2::Up;
	mRays[UP_LEFT].Mask = 1;
	mRays[UP_RIGHT].Position = HGF::Vector2(upSpread, 0.0f);
	mRays[UP_RIGHT].Direction = HGF::Vector2::Up;
	mRays[UP_RIGHT].Mask = 1;

	mRays[DOWN_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	mRays[DOWN_CENTER].Direction = HGF::Vector2::Down;
	mRays[DOWN_CENTER].Mask = 2;
	mRays[DOWN_LEFT].Position = HGF::Vector2(-downSpreed, 0.0f);
	mRays[DOWN_LEFT].Direction = HGF::Vector2::Down;
	mRays[DOWN_LEFT].Mask = 1;
	mRays[DOWN_RIGHT].Position = HGF::Vector2(downSpreed, 0.0f);
	mRays[DOWN_RIGHT].Direction = HGF::Vector2::Down;
	mRays[DOWN_RIGHT].Mask = 1;

	mRays[LEFT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	mRays[LEFT_CENTER].Direction = HGF::Vector2::Left;
	mRays[LEFT_CENTER].Mask = 2;
	mRays[LEFT_TOP].Position = HGF::Vector2(0.0f, -sideSpread);
	mRays[LEFT_TOP].Direction = HGF::Vector2::Left;
	mRays[LEFT_TOP].Mask = 1;
	mRays[LEFT_BOTTOM].Position = HGF::Vector2(0.0f, sideSpread);
	mRays[LEFT_BOTTOM].Direction = HGF::Vector2::Left;
	mRays[LEFT_BOTTOM].Mask = 1;

	mRays[RIGHT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	mRays[RIGHT_CENTER].Direction = HGF::Vector2::Right;
	mRays[RIGHT_CENTER].Mask = 2;
	mRays[RIGHT_TOP].Position = HGF::Vector2(0.0f, -sideSpread);
	mRays[RIGHT_TOP].Direction = HGF::Vector2::Right;
	mRays[RIGHT_TOP].Mask = 1;
	mRays[RIGHT_BOTTOM].Position = HGF::Vector2(0.0f, sideSpread);
	mRays[RIGHT_BOTTOM].Direction = HGF::Vector2::Right;
	mRays[RIGHT_BOTTOM].Mask = 1;

	m_MotorState = MotorState::Falling;
	m_CollisionSurface = CollisionSurface::None;

	m_MaxGroundSpeed = 0.0f;
	m_MaxAirSpeed = 0.0f;
	m_TimeToMaxGroundSpeed = 0.0f;
	m_TimeToMaxAirSpeed = 0.0f;
	m_DistanceForStopOnGround = 0.0f;
	m_DistanceForStopInAir = 0.0f;
	m_CanChangeDirectionInAir = true;
	m_MaxFallSpeed = 0.0f;
	m_MaxFastFallSpeed = 0.0f;
	m_FastFallGravityMultiplier = 0.0f;
	m_MinJumpHeight = 0.0f;
	m_MaxJumpHeight = 0.0f;
	m_JumpGraceTime = 0.0f;
	m_AirJumpCount = 0;
	m_CanWallJump = true;
	m_WallJumpSpeedMultiplier = 0.0f;
	m_CanWallCling = true;
	m_WallClingDuration = 0.0f;
	m_CanWallSlide = true;
	m_WallSlideSpeedMultiplier = 0.0f;
	m_CanGrabLedge = true;
	m_LedgeGrabJumpSpeedMultiplier = 0.0f;
	m_LedgeGrabDuration = 0.0f;
	m_LedgeGrabCheckDistance = 0.0f;
	m_CanDash = true;
	m_DashCooldown = 0.0f;
	m_DashDistance = 0.0f;
	m_DashDuration = 0.0f;
	m_DashEndDelay = 0.0f;

	m_NormalizedHorizontalMovement = 0.0f;
	m_PreviousPosition = HGF::Vector2::Zero;
}

Player::~Player()
{
}

bool Player::Load(const std::string& pFilename)
{
	if (!mSprite.Initialize(pFilename, "data/player.json"))
		return false;

	mSprite.SetState("Movement", "Idle");

	if (!DirectionalProjectile::DirProjTexture.Load("data/img/projectile.png", HGF::Interpolation::Nearest, HGF::Wrapping::ClampToBorder))
		return false;

	return true;
}

void Player::FireWeapon()
{
	DirectionalProjectile* proj = mManager->Create<DirectionalProjectile>(Entity::mWorld);
	proj->Position = Position;
	proj->Speed = 250.0f;
	proj->LifeTime = 2.5f;
	proj->FiredDirection = IsFacingLeft ? HGF::Vector2::Left : HGF::Vector2::Right;
}

void Player::Update(float pDeltaTime)
{
	Update_Input(pDeltaTime);
	Update_FixedUpdateA(pDeltaTime);
	Update_AfterTickA(pDeltaTime);
	//Update_FixedUpdateB(pDeltaTime);
	//Update_AfterTickB(pDeltaTime);
	
	if (IsOnGround)
	{
		mSprite.SetState("Movement", "Idle");
	}
	else
	{
		mSprite.SetState("Movement", "Jumping");
	}

	mSprite.Update(pDeltaTime);
}

void Player::Update_Input(float pDeltaTime)
{	
	// debug
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Backspace))
	{
		Position = HGF::Vector2(100.0f, 100.0f);
		Velocity = HGF::Vector2::Zero;
		Acceleration = HGF::Vector2::Zero;
		IsOnGround = false;
		IsJumping = false;
	}

	// Input: Jumping
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Z) && IsOnGround && !IsJumping)
	{
		mJumpTime = 0.0f;
		IsOnGround = false;
		IsJumping = true;
	}
	if (HGF::Keyboard::IsKeyUp(HGF::Key::Z) && IsJumping)
	{
		if (mJumpTime > mMinimumJumpTime)
		{
			IsJumping = false;
		}
	}

	// Input: Firing
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::X))
	{
		FireWeapon();
	}

	// Input: Movement
	if (HGF::Keyboard::IsKeyDown(HGF::Key::Left))
	{
		Acceleration.X -= MovementSpeed;
		IsFacingLeft = true;
	}
	if (HGF::Keyboard::IsKeyDown(HGF::Key::Right))
	{
		Acceleration.X += MovementSpeed;
		IsFacingLeft = false;
	}
}

void Player::Update_FixedUpdateA(float pDeltaTime)
{
	// update movement
	if (!IsOnGround)
	{
		Acceleration.Y += Gravity;
	}
	if (IsJumping)
	{
		Acceleration.Y -= JumpingSpeed;
		mJumpTime += pDeltaTime;
		if (mJumpTime > mMaximumJumpTime)
		{
			IsJumping = false;
		}
	}
	Velocity += Acceleration * pDeltaTime;
	Position += Velocity * pDeltaTime;
	Velocity.X *= 0.9f;
	Acceleration.X *= 0.9f;
}

void Player::Update_AfterTickA(float pDeltaTime)
{
	// Fire the rays.
	for (int i = 0; i < 12; ++i)
	{
		mWorld->Raycast(Position + mRays[i].Position, mRays[i].Direction, mRays[i].Mask, mRaycastDatas[i]);
	}

	float vertDistance = 35.0f;
	float horzDistance = 25.0f;

	// Up (0, 1, 2)
	for (int i = 0; i <= 2; ++i)
	{
		if (mRaycastDatas[i].Distance < vertDistance)
		{
			Position.Y = mRaycastDatas[i].EndPosition.Y + vertDistance;
			Velocity.Y = 0.0f;
			Acceleration.Y = 0.0f;
			break;
		}
	}

	// Down (3, 4, 5)
	if (mRaycastDatas[DOWN_CENTER].Distance < vertDistance + 5.0f && !IsJumping)
	{
		Position.Y = mRaycastDatas[DOWN_CENTER].EndPosition.Y - vertDistance;
		Velocity.Y = 0.0f;
		Acceleration.Y = 0.0f;
		IsOnGround = true;
		IsJumping = false;
	}
	else
	{
		IsOnGround = false;
	}

	// Left (6, 7, 8)
	for (int i = 6; i <= 8; ++i)
	{
		if (mRaycastDatas[i].Distance < horzDistance)
		{
			Position.X = mRaycastDatas[i].EndPosition.X + horzDistance;
			Velocity.X = 0.0f;
			Acceleration.X = 0.0f;
			break;
		}
	}

	// Right (9, 10, 11)
	for (int i = 9; i <= 11; ++i)
	{
		if (mRaycastDatas[i].Distance < horzDistance)
		{
			Position.X = mRaycastDatas[i].EndPosition.X - horzDistance;
			Velocity.X = 0.0f;
			Acceleration.X = 0.0f;
			break;
		}
	}
}

void Player::Update_FixedUpdateB(float pDeltaTime)
{

}

void Player::Update_AfterTickB(float pDeltaTime)
{

}

void Player::Render(SAGE::SpriteBatch& pSpriteBatch)
{	
	pSpriteBatch.Draw(mSprite.GetTexture(),
					  Position,
					  mSprite.GetRegion(),
					  HGF::Color::White,
					  mSprite.GetOrigin(),
					  0.0f,
					  Scale,
					  IsFacingLeft ? SAGE::Orientation::FlipHorizontal : SAGE::Orientation::None);
}

void Player::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	// AABB
	const HGF::Rectangle& dimensions = mSprite.GetRegion();
	HGF::Vector2 TL(Position.X - dimensions.Width / 2.0f * Scale.X, Position.Y - dimensions.Height / 2.0f * Scale.Y);
	HGF::Vector2 BR(Position.X + dimensions.Width / 2.0f * Scale.X, Position.Y + dimensions.Height / 2.0f * Scale.Y);
	pGeometryBatch.DrawRectangle(TL, BR, HGF::Color::Green);

	// Raycasts
	for (int i = 0; i < 12; ++i)
	{
		pGeometryBatch.DrawLine(mRaycastDatas[i].StartPosition, mRaycastDatas[i].EndPosition, HGF::Color::Yellow);
	}
}

void Player::Move(const HGF::Vector2& p_Movement)
{
	m_NormalizedHorizontalMovement = p_Movement.X / p_Movement.Length();
}

void Player::Jump()
{

}

void Player::Fire()
{

}

void Player::Dash()
{

}

void Player::Fall()
{
	if (m_MotorState == MotorState::Falling)
	{
		m_MotorState = MotorState::FastFalling;
	}
}

bool Player::HasFlag(CollisionSurface p_CollisionSurface) const
{
	return static_cast<CollisionSurface>(static_cast<int>(m_CollisionSurface) & static_cast<int>(p_CollisionSurface)) != CollisionSurface::None;
}

bool Player::IsGrounded() const
{
	return HasFlag(CollisionSurface::Floor) && Velocity.Y <= 0.0f;
}

bool Player::IsAirborne() const
{
	return m_MotorState == MotorState::Jumping || m_MotorState == MotorState::Falling || m_MotorState == MotorState::FastFalling;
}

// TODO: move to attribute
static const float WALL_INTERACTION_THRESHOLD = 0.5f;

bool Player::IsPressingIntoLeftWall() const
{
	return HasFlag(CollisionSurface::LeftWall) && m_NormalizedHorizontalMovement < -WALL_INTERACTION_THRESHOLD;
}

bool Player::IsPressingIntoRightWall() const
{
	return HasFlag(CollisionSurface::RightWall) && m_NormalizedHorizontalMovement > WALL_INTERACTION_THRESHOLD;
}

bool Player::IsAtLedge() const
{
	return false;
}

void Player::SetFacing()
{
	if (m_NormalizedHorizontalMovement < 0.0f)
	{
		IsFacingLeft = true;
	}
	else if (m_NormalizedHorizontalMovement > 0.0f)
	{
		IsFacingLeft = false;
	}
}

void Player::ClampFallSpeed()
{
	if (!IsGrounded())
	{
		if (m_MotorState == MotorState::FastFalling)
		{
			if (Velocity.Y < -m_MaxFastFallSpeed)
			{
				Velocity.Y = -m_MaxFastFallSpeed;
			}
		}
		else
		{
			if (Velocity.Y < -m_MaxFallSpeed)
			{
				Velocity.Y = -m_MaxFallSpeed;
			}
		}
	}
}

float Player::CalculateRequiredSpeed(float p_Height) const
{
	return sqrtf(-2.0f * p_Height * Gravity);
}

void Player::ApplyMovement(float p_DeltaTime)
{
	if (HGF::MathUtil::Abs(m_NormalizedHorizontalMovement))
	{
		if (IsGrounded())
		{
			if (m_TimeToMaxGroundSpeed > 0.0f)
			{
				// Decelerate if over max speed or trying to move in opposite direction.
				if ((Velocity.X > 0.0f && m_NormalizedHorizontalMovement > 0.0f && Velocity.X > m_NormalizedHorizontalMovement * m_MaxGroundSpeed) ||
					(Velocity.X < 0 && m_NormalizedHorizontalMovement < 0 && Velocity.X < m_NormalizedHorizontalMovement * m_MaxGroundSpeed) ||
					(Velocity.X < 0 && m_NormalizedHorizontalMovement > 0) ||
					(Velocity.X > 0 && m_NormalizedHorizontalMovement < 0))
				{
					Decelerate((m_MaxGroundSpeed * m_MaxGroundSpeed) / (2.0f * m_DistanceForStopOnGround), m_NormalizedHorizontalMovement * m_MaxGroundSpeed, p_DeltaTime);
				}
				else
				{
					Accelerate(m_NormalizedHorizontalMovement * (m_MaxGroundSpeed / m_TimeToMaxGroundSpeed), m_NormalizedHorizontalMovement * m_MaxGroundSpeed, p_DeltaTime);
				}
			}
			else
			{
				Velocity = HGF::Vector2::Right * m_NormalizedHorizontalMovement * m_MaxGroundSpeed;
			}
		}
		else if (m_CanChangeDirectionInAir)
		{
			if (m_TimeToMaxAirSpeed > 0.0f)
			{
				// Decelerate if over max speed or trying to move in opposite direction.
				if ((Velocity.X > 0 && m_NormalizedHorizontalMovement > 0 && Velocity.X > m_NormalizedHorizontalMovement * m_MaxAirSpeed) ||
					(Velocity.X < 0 && m_NormalizedHorizontalMovement < 0 && Velocity.X < m_NormalizedHorizontalMovement * m_MaxAirSpeed))
				{
					Decelerate((m_MaxAirSpeed * m_MaxAirSpeed) / (2.0f * m_DistanceForStopInAir), m_NormalizedHorizontalMovement * m_MaxAirSpeed, p_DeltaTime);
				}
				else
				{
					Accelerate(m_NormalizedHorizontalMovement * (m_MaxAirSpeed / m_TimeToMaxAirSpeed), m_NormalizedHorizontalMovement * m_MaxAirSpeed, p_DeltaTime);
				}
			}
			else
			{
				Velocity.X = m_NormalizedHorizontalMovement * m_MaxAirSpeed;
			}
		}
	}
	else if (Velocity.X != 0.0f)
	{
		if (IsGrounded())
		{
			if (m_DistanceForStopOnGround > 0)
			{
				Decelerate((m_MaxGroundSpeed * m_MaxGroundSpeed) / (2.0f * m_DistanceForStopOnGround), 0.0f, p_DeltaTime);
			}
			else
			{
				Velocity = HGF::Vector2::Zero;
			}
		}
		else
		{
			if (m_DistanceForStopInAir > 0)
			{
				Decelerate((m_MaxAirSpeed * m_MaxAirSpeed) / (2.0f * m_DistanceForStopInAir), 0.0f, p_DeltaTime);
			}
			else
			{
				Velocity.X = 0.0f;
			}
		}
	}
}

void Player::Accelerate(float p_Acceleration, float p_Limit, float p_DeltaTime)
{
	Velocity.X += p_Acceleration * p_DeltaTime;

	if (p_Acceleration > 0.0f)
	{
		if (Velocity.X > p_Limit)
		{
			Velocity.X = p_Limit;
		}
	}
	else
	{
		if (Velocity.X < p_Limit)
		{
			Velocity.X = p_Limit;
		}
	}
}

void Player::Decelerate(float p_Deceleration, float p_Limit, float p_DeltaTime)
{
	if (Velocity.X < 0.0f)
	{
		Velocity.X += p_Deceleration * p_DeltaTime;

		if (Velocity.X > p_Limit)
		{
			Velocity.X = p_Limit;
		}
	}
	else if (Velocity.X > 0.0f)
	{
		Velocity.X -= p_Deceleration * p_DeltaTime;

		if (Velocity.X < p_Limit)
		{
			Velocity.X = p_Limit;
		}
	}
}

void Player::StartDash(float p_DeltaTime)
{
	// Dash mantains the same direction the entire time.
	SetFacing();

	if (!m_DashState.IsWithDirection)
	{
		// Dependent on facing direction.
		m_DashState.Direction = IsFacingLeft ? HGF::Vector2::Left : HGF::Vector2::Right;
	}

	m_DashState.DistanceDashed = 0.0f;
	m_DashState.DistanceCalculated = 0.0f;
	m_DashState.TimeDashed = p_DeltaTime;
	m_PreviousPosition = Position;
	m_MotorState = MotorState::Dashing;

	//if (OnDash != null) { OnDash(); }
}

void Player::HandleDash(float p_DeltaTime)
{
	m_DashState.TimeDashed = HGF::MathUtil::Clamp<float>(m_DashState.TimeDashed + p_DeltaTime, 0.0f, m_DashDuration);

	float normalizedTime = m_DashState.TimeDashed / m_DashDuration;
	float distance = _dashFunction(0.0f, m_DashDistance, normalizedTime);
	
	m_PreviousPosition = Position;
	Position = Position + m_DashState.Direction * (distance - m_DashState.DistanceCalculated);
	m_DashState.DistanceCalculated = distance;
}

void Player::HandlePreWallInteraction()
{
	// If told to fall fast or jump, then don't allow any wall interactions.
	if (m_MotorState == MotorState::FastFalling || m_MotorState == MotorState::Jumping)
	{
		return;
	}

	// Ledge grabbing
	if (m_CanGrabLedge)
	{
		if (Velocity.Y <= 0.0f && IsAtLedge() && m_WallState.CanHangAgain)
		{
			m_WallState.LedgeGrabTime = m_LedgeGrabDuration;
			m_WallState.CanHangAgain = false;
			Velocity = HGF::Vector2::Zero;
			m_MotorState = MotorState::LedgeGrabbing;
			return;
		}
	}

	// Wall clinging
	if (m_CanWallCling)
	{
		if (Velocity.Y <= 0.0f && (IsPressingIntoLeftWall() || IsPressingIntoRightWall()) && m_WallState.CanHangAgain)
		{
			m_WallState.WallClingTime = m_WallClingDuration;
			m_WallState.CanHangAgain = false;
			Velocity = HGF::Vector2::Zero;
			m_MotorState = MotorState::WallClinging;
			return;
		}
	}

	// Wall sliding
	if (m_CanWallSlide && m_MotorState != MotorState::WallClinging && m_MotorState != MotorState::LedgeGrabbing)
	{
		if (Velocity.Y <= 0.0f && (IsPressingIntoLeftWall() || IsPressingIntoRightWall()))
		{
			Velocity = HGF::Vector2::Down * m_WallSlideSpeedMultiplier;
			m_MotorState = MotorState::WallSliding;
			return;
		}
	}
}

void Player::HandlePostWallInteraction()
{
	// Check away from walls.
	if (!IsPressingIntoLeftWall() && !IsPressingIntoRightWall())
	{
		m_WallState.CanHangAgain = true;
	}

	// Kill off horizontal velocity if by a wall.
	if ((HasFlag(CollisionSurface::LeftWall) && Velocity.X < 0.0f) || (HasFlag(CollisionSurface::RightWall) && Velocity.X > 0.0f))
	{
		Velocity.X = 0.0f;
	}

	// Check done ledge grabbing.
	if (m_MotorState == MotorState::LedgeGrabbing)
	{
		if (!IsAtLedge() || m_WallState.LedgeGrabTime < 0)
		{
			m_MotorState = MotorState::Falling;
		}
	}

	// Check done ledge grabbing.
	if (m_MotorState == MotorState::WallClinging)
	{
		if (!(IsPressingIntoLeftWall() || IsPressingIntoRightWall()) || m_WallState.LedgeGrabTime < 0.0f)
		{
			m_MotorState = MotorState::Falling;
		}
	}

	// ???
	if (m_MotorState == MotorState::Falling && m_CanWallSlide && Velocity.Y <= 0.0f)
	{
		m_MotorState = MotorState::WallSliding;
	}

	// Check done wall sliding.
	if (m_MotorState == MotorState::WallSliding)
	{
		if (!(IsPressingIntoLeftWall() || IsPressingIntoRightWall()))
		{
			m_MotorState = MotorState::Falling;
		}
	}

	// Check if on ground.
	if (HasFlag(CollisionSurface::Floor))
	{
		m_MotorState = MotorState::Grounded;
	}
}