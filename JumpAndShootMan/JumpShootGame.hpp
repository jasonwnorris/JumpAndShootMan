// JumpShootGame.hpp

#ifndef __GAME_HPP__
#define __GAME_HPP__

#define BATCH_RENDERING 1

#include "Globals.hpp"
// SAGE Includes
#include <SAGE\Game.hpp>
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Effect.hpp>
#include <HGF\Window.hpp>
// Project Includes
#include "Renderer.hpp"
#include "TiledMap.hpp"
#include "Player.hpp"
// STL Includes
#include <map>

class JumpShootGame : public SAGE::Game
{
	public:
		JumpShootGame();
		~JumpShootGame();

	protected:
		int Initialize() override;
		int Finalize() override;
		int Update(float pDeltaTime) override;
		int Render() override;

	private:
		void UpdateRaycast();
		void UpdateAdjustment(Direction pDirection, float pDistance);

		int mFrameCount;
		Uint32 mPreviousTicks;
		Uint32 mCurrentTicks;

		int mWindowWidth;
		int mWindowHeight;

		HGF::Window mWindow;
		SAGE::GeometryBatch mGeometryBatch;
		HGF::Effect mGeometryEffect;
		SAGE::SpriteBatch mSpriteBatch;
		HGF::Effect mSpriteEffect;
		Renderer mRenderer;
		SAGE::SpriteFont mSpriteFont;
		TiledMap mTiledMap;
		EntityManager mEntityManager;
		std::shared_ptr<Player> mPlayer;
};

#endif