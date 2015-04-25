// Projectile.hpp

#ifndef __PROJECTILE_HPP__
#define __PROJECTILE_HPP__

// Project Includes
#include "EntityManager.hpp"

class Projectile : public Entity
{
	public:
		Projectile(EntityManager* pManager);
		~Projectile();

		virtual void Update(float pDeltaTime) override = 0;
		virtual void Render(const Renderer& pRenderer) override = 0;
		virtual void Render(SAGE::SpriteBatch& pSpriteBatch) override = 0;

	public:
		HGF::Vector2 Position;
		float Speed;
		float LifeTime;
};

#endif