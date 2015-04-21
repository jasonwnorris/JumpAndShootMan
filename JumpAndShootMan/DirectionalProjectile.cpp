// DirectionalProjectile.cpp

// Project Includes
#include "DirectionalProjectile.hpp"

HGF::Texture DirectionalProjectile::DirProjTexture;

DirectionalProjectile::DirectionalProjectile(EntityManager* pManager) : Projectile(pManager)
{
}

DirectionalProjectile::~DirectionalProjectile()
{
}

void DirectionalProjectile::Update(float pDeltaTime)
{
	switch (Direction)
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
}

void DirectionalProjectile::Render(const Renderer& pRenderer)
{

}

void DirectionalProjectile::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	pSpriteBatch.Draw(DirProjTexture, Position, HGF::Rectangle::Empty, HGF::Color::White, HGF::Vector2::Zero, 0.0f, HGF::Vector2::One, SAGE::Orientation::None);
}