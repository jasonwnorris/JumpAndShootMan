#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Timer.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "Map.hpp"
#include "Player.hpp"

#include <map>

class Game
{
	public:
		Game();
		~Game();

		int Run();

	private:
		int Initialize();
		int Finalize();
		int HandleInput();
		int Update(float pDeltaTime);
		int Render();

		void UpdateRaycast();
		void UpdateAdjustment(Direction pDirection, float pDistance);

		int mFrameCount;
		Uint32 mPreviousTicks;
		Uint32 mCurrentTicks;

		int mWindowWidth;
		int mWindowHeight;

		SDL_Event mEvent;
		std::map<SDL_Keycode, bool> mKeys;
		SDL_Window* mWindow;
		SDL_GLContext mContext;
		bool mIsRunning;
		Timer mTimer;
		Camera mCamera;
		Renderer mRenderer;
		Map mMap;
		Player mPlayer;
};

#endif