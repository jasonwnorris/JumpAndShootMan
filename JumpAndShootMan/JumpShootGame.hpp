// JumpShootGame.hpp

#ifndef __JUMPSHOOTGAME_HPP__
#define __JUMPSHOOTGAME_HPP__

#include "Globals.hpp"
// SAGE Includes
#include <SAGE\Game.hpp>
#include <SAGE\ScreenManager.hpp>
// Project Includes
#include "GameplayScreen.hpp"

class JumpShootGame : public SAGE::Game
{
	public:
		JumpShootGame();
		~JumpShootGame();

	protected:
		int Initialize() override;
		int Finalize() override;
		int Update(float pDeltaTime) override;
		int Render(SAGE::SpriteBatch& pSpriteBatch) override;
		int Render(SAGE::GeometryBatch& pGeometryBatch) override;

	private:
		SAGE::ScreenManager mScreenManager;
		int mFrameCount;
		Uint32 mPreviousTicks;
		Uint32 mCurrentTicks;
		int mSBDrawCalls;
		int mGBDrawCalls;
};

#endif