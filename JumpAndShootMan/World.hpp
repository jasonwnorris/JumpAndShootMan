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

		int Initialize();
		int Finalize();

		int Update(float pDeltaTime);
		int Render(SAGE::SpriteBatch& pSpriteBatch);
		int Render(SAGE::GeometryBatch& pGeometryBatch);

		template<typename T> T* Create();
		void Raycast(const HGF::Vector2& pPosition, const HGF::Vector2& pDirection, int pLayerMask, RaycastData& pRaycastData);

	private:
		TiledMap mTiledMap;
		EntityManager mEntityManager;
};

template<typename T> T* World::Create()
{
	return mEntityManager.Create<T>(this);
}

#endif