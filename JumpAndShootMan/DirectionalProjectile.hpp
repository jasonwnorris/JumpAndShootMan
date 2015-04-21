// DirectionalProjectile.hpp

#ifndef __DIRECTIONALPROJECTILE_HPP__
#define __DIRECTIONALPROJECTILE_HPP__

// Project Includes
#include "Direction.hpp"
#include "Projectile.hpp"

class DirectionalProjectile : public Projectile
{
	public:
		static HGF::Texture DirProjTexture;

	public:
		DirectionalProjectile(EntityManager* pManager);
		~DirectionalProjectile();

		void Update(float pDeltaTime) override;
		void Render(const Renderer& pRenderer) override;
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;

	public:
		Direction Direction;
};

#endif