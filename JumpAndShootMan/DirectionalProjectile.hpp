// DirectionalProjectile.hpp

#ifndef __DIRECTIONALPROJECTILE_HPP__
#define __DIRECTIONALPROJECTILE_HPP__

// Project Includes
#include "ITiledMapCollider.hpp"
#include "Projectile.hpp"

class DirectionalProjectile : public Projectile, public ITiledMapCollider
{
	public:
		static HGF::Texture DirProjTexture;

	public:
		DirectionalProjectile(EntityManager* pManager, World* pWorld);
		~DirectionalProjectile();

		void OnCollision(Direction pDirection, float pDistance) override;
		void OnEmpty(Direction pDirection, float pDistance) override;

		void Update(float pDeltaTime) override;
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;
		void Render(SAGE::GeometryBatch& pGeometryBatch) override;

	public:
		Direction FiredDirection;
		HGF::Vector2 Dimensions;
};

#endif