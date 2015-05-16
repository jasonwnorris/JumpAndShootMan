// Player.cpp

// HGF Includes
#include <HGF\Keyboard.hpp>
// Project Includes
#include "Globals.hpp"
#include "Player.hpp"
#include "DirectionalProjectile.hpp"
#include "World.hpp"

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
	IsGrounded = false;
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
	// debug
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Backspace))
	{
		Position = HGF::Vector2(100.0f, 100.0f);
		Velocity = HGF::Vector2::Zero;
		Acceleration = HGF::Vector2::Zero;
		IsGrounded = false;
		IsJumping = false;
	}

	// Input: Jumping
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Z) && IsGrounded && !IsJumping)
	{
		mJumpTime = 0.0f;
		IsGrounded = false;
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

	// update movement
	if (!IsGrounded)
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
		IsGrounded = true;
		IsJumping = false;
	}
	else
	{
		IsGrounded = false;
	}
	/*
	else if (mRaycastDatas[DOWN_LEFT].Distance < vertDistance)
	{
		Position.Y = mRaycastDatas[DOWN_LEFT].EndPosition.Y - vertDistance;
		Velocity.Y = 0.0f;
		Acceleration.Y = 0.0f;
		IsGrounded = true;
	}
	else if (mRaycastDatas[DOWN_RIGHT].Distance < vertDistance)
	{
		Position.Y = mRaycastDatas[DOWN_RIGHT].EndPosition.Y - vertDistance;
		Velocity.Y = 0.0f;
		Acceleration.Y = 0.0f;
		IsGrounded = true;
	}
	else if (!IsJumping)
	{
		IsGrounded = false;
	}
	*/

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

	if (IsGrounded)
	{
		mSprite.SetState("Movement", "Idle");
	}
	else
	{
		mSprite.SetState("Movement", "Jumping");
	}

	mSprite.Update(pDeltaTime);
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