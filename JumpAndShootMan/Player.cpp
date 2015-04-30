// Player.cpp

// Project Includes
#include "Globals.hpp"
#include "Player.hpp"
#include "DirectionalProjectile.hpp"

Player::Player(EntityManager* pManager, World* pWorld) : Entity(pManager, pWorld), ITiledMapCollider(pWorld)
{
	Position = HGF::Vector2(100.0f, 100.0f);
	Velocity = HGF::Vector2::Zero;
	Acceleration = HGF::Vector2::Zero;
	IsFacingLeft = false;
	MovementSpeed = 0.195f;
	JumpingSpeed = 1.75f;
	Gravity = 0.055f;
	Source = HGF::Rectangle(9, 107, 110, 150);
	Scale = HGF::Vector2(0.5f, 0.5f);
	//Scale = HGF::Vector2::One;
	Origin = HGF::Vector2(Source.Width / 2.0f, Source.Height / 2.0f);
	Dimensions = HGF::Vector2(Source.Width * Scale.X, Source.Height * Scale.Y);
	IsGrounded = false;
	IsJumping = false;
	IsDebugFly = false;
}

Player::~Player()
{
}

bool Player::Load(const std::string& pFilename)
{
	if (!mTexture.Load(pFilename.c_str(), HGF::Interpolation::Nearest, HGF::Wrapping::ClampToBorder))
		return false;

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
	float vertMin = Dimensions.Y / 2.0f;
	float vertMax = vertMin + 5.0f;
	float horzMin = Dimensions.X / 2.0f;
	float horzMax = horzMin;

	SetTriRay(Direction::Up, Position, Dimensions.X / 2.0f, vertMin, vertMax);
	SetTriRay(Direction::Down, Position, Dimensions.X / 1.5f, vertMin, vertMax);
	SetTriRay(Direction::Left, Position, Dimensions.Y / 2.0f, horzMin, horzMax);
	SetTriRay(Direction::Right, Position, Dimensions.Y / 2.0f, horzMin, horzMax);

	CheckCollision();
}

void Player::Render(SAGE::SpriteBatch& pSpriteBatch)
{	
	pSpriteBatch.Draw(mTexture,
					  Position,
					  Source,
					  HGF::Color::White,
					  Origin,
					  0.0f,
					  Scale,
					  IsFacingLeft ? SAGE::Orientation::FlipHorizontal : SAGE::Orientation::None);
}

void Player::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	// AABB
	HGF::Vector2 TL(Position.X - Dimensions.X / 2.0f, Position.Y - Dimensions.Y / 2.0f);
	HGF::Vector2 BR(Position.X + Dimensions.X / 2.0f, Position.Y + Dimensions.Y / 2.0f);
	pGeometryBatch.Draw(HGF::Vector2(TL.X, TL.Y), HGF::Vector2(BR.X, TL.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(BR.X, TL.Y), HGF::Vector2(BR.X, BR.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(BR.X, BR.Y), HGF::Vector2(TL.X, BR.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(TL.X, BR.Y), HGF::Vector2(TL.X, TL.Y), HGF::Color::Green);

	// Collision Rays
	ITiledMapCollider::Render(pGeometryBatch);
}