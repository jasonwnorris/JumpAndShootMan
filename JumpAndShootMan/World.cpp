// World.cpp

#include "Globals.hpp"
// Project Includes
#include "World.hpp"

World::World()
{
}

World::~World()
{
}

const TiledMap* World::GetMap() const
{
	return &mTiledMap;
}

int World::Initialize()
{
	// Load the map.
	if (!mTiledMap.Load("data/maps/first.json"))
		return -1;

	return 0;
}

int World::Finalize()
{
	return 0;
}

int World::Update(float pDeltaTime)
{
	mEntityManager.Update(pDeltaTime);

	return 0;
}

int World::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	mTiledMap.Render(pSpriteBatch);
	mEntityManager.Render(pSpriteBatch);

	return 0;
}

int World::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	mTiledMap.Render(pGeometryBatch);
	mEntityManager.Render(pGeometryBatch);

	return 0;
}

void World::Raycast(const HGF::Vector2& pPosition, const HGF::Vector2& pDirection, int pLayerMask, RaycastData& pRaycastData)
{
	mTiledMap.Raycast(pPosition, pDirection, pLayerMask, pRaycastData);
}