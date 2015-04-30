// World.cpp

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

void World::Initialize()
{
	// Load the map.
	mTiledMap.Load("data/maps/first.txt");
}

void World::Finalize()
{
}

void World::Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit)
{
	mTiledMap.Raycast(pPosition, pDirection, pHasInterest, pRaycastHit);
}

void World::Update(float pDeltaTime)
{
	mEntityManager.Update(pDeltaTime);
}

void World::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	mTiledMap.Render(pSpriteBatch);
	mEntityManager.Render(pSpriteBatch);
}

void World::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	mTiledMap.Render(pGeometryBatch);
	mEntityManager.Render(pGeometryBatch);
}