#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <SDL2\SDL.h>

class Timer
{
	public:
		Timer();
		~Timer();

		void Update();

		float GetDeltaTime();
		float GetTotalTime();

	private:
		Uint32 mTotalTime;
		Uint32 mElapsedTime;
		Uint32 mLastTime;
};

#endif