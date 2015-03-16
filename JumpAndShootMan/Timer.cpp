#include "Timer.hpp"

Timer::Timer()
{
	mTotalTime = 0;
	mElapsedTime = 0;
	mLastTime = SDL_GetTicks();
}

Timer::~Timer()
{
}

void Timer::Update()
{
	Uint32 timeNow = SDL_GetTicks();
	mElapsedTime = timeNow - mLastTime;
	mTotalTime += mElapsedTime;
	mLastTime = timeNow;
}

float Timer::GetDeltaTime()
{
	return (float)(mElapsedTime / 1000.0f);
}

float Timer::GetTotalTime()
{
	return (float)(mTotalTime / 1000.0f);
}