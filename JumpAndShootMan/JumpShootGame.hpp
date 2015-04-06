// JumpShootGame.hpp

#ifndef __GAME_HPP__
#define __GAME_HPP__

// SAGE Includes
#include <SAGE\Game.hpp>
// Project Includes
#include "Camera.hpp"
#include "Renderer.hpp"
#include "Map.hpp"
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

		SDL_Window* mWindow;
		SDL_GLContext mContext;
		Camera mCamera;
		Renderer mRenderer;
		Map mMap;
		Player mPlayer;
};

#endif