// World.cpp

#include "Globals.hpp"
// Project Includes
#include "World.hpp"

World::World()
{
	mTime = 0.0f;
	mText = "Jump 'n' Shoot Man!";
	mPosition = HGF::Vector2(25.0f * 32.0f / 2.0f, 15.0f * 32.0f / 4.0f);
	mOrigin = HGF::Vector2::Zero;
	mRotation = 0.0f;
	mScale = 0.0f;
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
	// Load the sprite font.
	if (!mSpriteFont.Load("data/img/font.png", 26.0f, 16.0f))
		return -1;

	mSpriteFont.MeasureString(mText, mOrigin);
	mOrigin /= 4.0f;

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

	mTime += pDeltaTime;
	mRotation = cosf(mTime * 3.0f) / 4;
	mScale = 0.8f + sinf(mTime * 2.0f) / 6.0f;

	return 0;
}

int World::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	mTiledMap.Render(pSpriteBatch);
	pSpriteBatch.DrawString(mSpriteFont, mText, mPosition, HGF::Color::White, mOrigin, mRotation, HGF::Vector2::One * mScale, SAGE::Orientation::None);
	mEntityManager.Render(pSpriteBatch);

	return 0;
}

int World::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	mTiledMap.Render(pGeometryBatch);
	mEntityManager.Render(pGeometryBatch);

	return 0;
}

void World::Raycast(const HGF::Vector2& pPosition, Direction pDirection, bool pHasInterest, RaycastHit& pRaycastHit)
{
	mTiledMap.Raycast(pPosition, pDirection, pHasInterest, pRaycastHit);
}