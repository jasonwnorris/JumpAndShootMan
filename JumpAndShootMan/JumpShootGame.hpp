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
		int Update(float p_DeltaTime) override;
		int Render(SAGE::SpriteBatch& p_SpriteBatch) override;
		int Render(SAGE::GeometryBatch& p_GeometryBatch) override;

	private:
		SAGE::ScreenManager m_ScreenManager;
		int m_FrameCount;
		Uint32 m_PreviousTicks;
		Uint32 m_CurrentTicks;
		int m_SBDrawCalls;
		int m_GBDrawCalls;
};

#endif
