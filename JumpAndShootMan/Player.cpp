// Player.cpp

// HGF Includes
#include <HGF\Keyboard.hpp>
// Project Includes
#include "Globals.hpp"
#include "Player.hpp"
#include "DirectionalProjectile.hpp"

Player::Player(EntityManager* pManager, World* pWorld) : Entity(pManager, pWorld), ITiledMapCollider(pWorld)
{
	Position = HGF::Vector2(100.0f, 100.0f);
	Velocity = HGF::Vector2::Zero;
	Acceleration = HGF::Vector2::Zero;
	Rotation = 0.0f;
	Scale = HGF::Vector2::One * 0.5f;
	MovementSpeed = 0.195f;
	JumpingSpeed = 1.75f;
	Gravity = 0.055f;
	IsFacingLeft = false;
	IsGrounded = false;
	IsJumping = false;
	IsDebugFly = false;
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

void Player::Fire()
{
	// TODO: Ambiguouis reference to World.
	DirectionalProjectile* proj = mManager->Create<DirectionalProjectile>(Entity::mWorld);
	proj->Position = Position;
	proj->Speed = 250.0f;
	proj->LifeTime = 2.5f;
	proj->FiredDirection = IsFacingLeft ? Direction::Left : Direction::Right;
}

void Player::OnCollision(Direction pDirection, float pDistance)
{
	switch (pDirection)
	{
		case Direction::Up:
			if (Velocity.Y < 0.0f)
			{
				Position.Y += pDistance;
				Velocity.Y = 0.0f;
				Acceleration.Y = 0.0f;
			}
			break;
		case Direction::Down:
			if (IsGrounded)
			{
				Position.Y -= pDistance;
				Velocity.Y = 0.0f;
				Acceleration.Y = 0.0f;
			}
			else if (Velocity.Y > 0.0f)
			{
				IsGrounded = true;
			}
			break;
		case Direction::Left:
			if (Velocity.X < 0.0f)
			{
				Position.X += pDistance;
				Velocity.X = 0.0f;
				Acceleration.X = 0.0f;
			}
			break;
		case Direction::Right:
			if (Velocity.X > 0.0f)
			{
				Position.X -= pDistance;
				Velocity.X = 0.0f;
				Acceleration.X = 0.0f;
			}
			break;
	}
}

void Player::OnEmpty(Direction pDirection, float pDistance)
{
	switch (pDirection)
	{
		case Direction::Up:
			break;
		case Direction::Down:
			IsGrounded = false;
			break;
		case Direction::Left:
			break;
		case Direction::Right:
			break;
	}
}

void Player::Update(float pDeltaTime)
{
	// Exception: Adding a new entity during the Update loop.

	const HGF::Rectangle& dimensions = mSprite.GetRegion();
	float vertMin = dimensions.Height / 2.0f * Scale.Y;
	float vertMax = vertMin + 5.0f;
	float horzMin = dimensions.Width / 2.0f * Scale.X;
	float horzMax = horzMin;

	SetTriRay(Direction::Up, Position, dimensions.Width / 2.0f * Scale.X, vertMin, vertMax);
	SetTriRay(Direction::Down, Position, dimensions.Width / 1.5f * Scale.X, vertMin, vertMax);
	SetTriRay(Direction::Left, Position, dimensions.Height / 2.0f * Scale.Y, horzMin, horzMax);
	SetTriRay(Direction::Right, Position, dimensions.Height / 2.0f * Scale.Y, horzMin, horzMax);

	CheckCollision();

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

	// Collision Rays
	ITiledMapCollider::Render(pGeometryBatch);
}