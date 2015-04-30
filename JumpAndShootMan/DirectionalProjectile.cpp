// DirectionalProjectile.cpp

// Project Includes
#include "Direction.hpp"
#include "DirectionalProjectile.hpp"

HGF::Texture DirectionalProjectile::DirProjTexture;

DirectionalProjectile::DirectionalProjectile(EntityManager* pManager, World* pWorld) : Projectile(pManager, pWorld), ITiledMapCollider(pWorld)
{
	Dimensions = HGF::Vector2(16.0f, 8.0f);
}

DirectionalProjectile::~DirectionalProjectile()
{
}

void DirectionalProjectile::OnCollision(Direction pDirection, float pDistance)
{
	mIsAlive = false;
}

void DirectionalProjectile::OnEmpty(Direction pDirection, float pDistance)
{
}

void DirectionalProjectile::Update(float pDeltaTime)
{
	switch (FiredDirection)
	{
		case Direction::Up:
			Position.Y -= Speed * pDeltaTime;
			break;
		case Direction::Down:
			Position.Y += Speed * pDeltaTime;
			break;
		case Direction::Left:
			Position.X -= Speed * pDeltaTime;
			break;
		case Direction::Right:
			Position.X += Speed * pDeltaTime;
			break;
	}

	LifeTime -= pDeltaTime;
	if (LifeTime < 0.0f)
		mIsAlive = false;

	float vertMax = Dimensions.Y / 2.0f;
	float vertMin = vertMax - 2.0f;
	float horzMax = Dimensions.X / 2.0f;
	float horzMin = horzMax - 2.0f;

	SetTriRay(Direction::Up, Position, Dimensions.X / 2.0f, vertMin, vertMax);
	SetTriRay(Direction::Down, Position, Dimensions.X / 2.0f, vertMin, vertMax);
	SetTriRay(Direction::Left, Position, Dimensions.Y / 2.0f, horzMin, horzMax);
	SetTriRay(Direction::Right, Position, Dimensions.Y / 2.0f, horzMin, horzMax);

	CheckCollision();
}

void DirectionalProjectile::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	pSpriteBatch.Draw(DirProjTexture, Position, HGF::Rectangle::Empty, HGF::Color::White, Dimensions / 2.0f, 0.0f, HGF::Vector2::One, SAGE::Orientation::None);
}

void DirectionalProjectile::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	HGF::Vector2 TL(Position.X - Dimensions.X / 2.0f, Position.Y - Dimensions.Y / 2.0f);
	HGF::Vector2 BR(Position.X + Dimensions.X / 2.0f, Position.Y + Dimensions.Y / 2.0f);
	pGeometryBatch.Draw(HGF::Vector2(TL.X, TL.Y), HGF::Vector2(BR.X, TL.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(BR.X, TL.Y), HGF::Vector2(BR.X, BR.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(BR.X, BR.Y), HGF::Vector2(TL.X, BR.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(TL.X, BR.Y), HGF::Vector2(TL.X, TL.Y), HGF::Color::Green);

	ITiledMapCollider::Render(pGeometryBatch);
}