// DirectionalProjectile.hpp

#ifndef __DIRECTIONALPROJECTILE_HPP__
#define __DIRECTIONALPROJECTILE_HPP__

// Project Includes
#include "Projectile.hpp"

class DirectionalProjectile : public Projectile
{
	public:
		static HGF::Texture DirProjTexture;

	public:
		DirectionalProjectile(EntityManager* pManager, World* pWorld);
		~DirectionalProjectile();

		void Update(float pDeltaTime) override;
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;
		void Render(SAGE::GeometryBatch& pGeometryBatch) override;

	public:
		HGF::Vector2 FiredDirection;
		HGF::Vector2 Dimensions;
};

#endif