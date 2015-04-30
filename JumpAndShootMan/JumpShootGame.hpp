// JumpShootGame.hpp

#ifndef __JUMPSHOOTGAME_HPP__
#define __JUMPSHOOTGAME_HPP__

#include "Globals.hpp"
// SAGE Includes
#include <SAGE\Game.hpp>
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
		void UpdateRaycast();
		void UpdateAdjustment(Direction pDirection, float pDistance);

		int mFrameCount;
		Uint32 mPreviousTicks;
		Uint32 mCurrentTicks;

		int mSBDrawCalls;
		int mGBDrawCalls;

		GameplayScreen mScreen;
};

#endif