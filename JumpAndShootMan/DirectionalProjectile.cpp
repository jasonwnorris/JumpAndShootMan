// DirectionalProjectile.cpp

// Project Includes
#include "Direction.hpp"
#include "DirectionalProjectile.hpp"

HGF::Texture DirectionalProjectile::DirProjTexture;

DirectionalProjectile::DirectionalProjectile(EntityManager* pManager, World* pWorld) : Projectile(pManager, pWorld)
{
	Dimensions = HGF::Vector2(16.0f, 8.0f);
}

DirectionalProjectile::~DirectionalProjectile()
{
}

void DirectionalProjectile::Update(float pDeltaTime)
{
	Position += FiredDirection * Speed * pDeltaTime;

	LifeTime -= pDeltaTime;
	if (LifeTime < 0.0f)
		mIsAlive = false;

	float vertMax = Dimensions.Y / 2.0f;
	float vertMin = vertMax - 2.0f;
	float horzMax = Dimensions.X / 2.0f;
	float horzMin = horzMax - 2.0f;
}

void DirectionalProjectile::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	pSpriteBatch.Draw(DirProjTexture, Position, HGF::Rectangle::Empty, HGF::Color::White, Dimensions / 2.0f, 0.0f, HGF::Vector2::One, SAGE::Orientation::None);
}

void DirectionalProjectile::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	HGF::Vector2 TL(Position.X - Dimensions.X / 2.0f, Position.Y - Dimensions.Y / 2.0f);
	HGF::Vector2 BR(Position.X + Dimensions.X / 2.0f, Position.Y + Dimensions.Y / 2.0f);
	pGeometryBatch.DrawRectangle(TL, BR, HGF::Color::Green);
}