// World.hpp

#ifndef __WORLD_HPP__
#define __WORLD_HPP__

// Project Includes
#include "TiledMap.hpp"
#include "EntityManager.hpp"

class World
{
	public:
		World();
		~World();

		const TiledMap* GetMap() const;

		void Initialize();
		void Finalize();

		void Update(float pDeltaTime);
		void Render(SAGE::SpriteBatch& pSpriteBatch);
		void Render(SAGE::GeometryBatch& pGeometryBatch);

		template<typename T> T* Create();
		void Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit);

	private:
		TiledMap mTiledMap;
		EntityManager mEntityManager;
};

template<typename T> T* World::Create()
{
	return mEntityManager.Create<T>(this);
}

#endif