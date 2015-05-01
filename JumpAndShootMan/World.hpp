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
		void Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit);

	private:
		SAGE::SpriteFont mSpriteFont;
		TiledMap mTiledMap;
		EntityManager mEntityManager;

		float mTime;
		std::string mText;
		HGF::Vector2 mPosition;
		HGF::Vector2 mOrigin;
		float mRotation;
		float mScale;
};

template<typename T> T* World::Create()
{
	return mEntityManager.Create<T>(this);
}

#endif