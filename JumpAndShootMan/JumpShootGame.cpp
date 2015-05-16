// JumpShootGame.cpp

// HGF Includes
#include <HGF\Events.hpp>
#include <HGF\Keyboard.hpp>
// Project Includes
#include "JumpShootGame.hpp"
// STL Includes
#include <iostream>
#include <time.h>

JumpShootGame::JumpShootGame()
{
	Globals::IsDebugDrawOn = false;

	mSBDrawCalls = 0;
	mGBDrawCalls = 0;
}

JumpShootGame::~JumpShootGame()
{
}

int JumpShootGame::Initialize()
{
	if (SAGE::Game::Initialize() < 0)
		return -1;

	HGF::Events::OnQuit.Add([&]() { Quit(); });

	HGF::WindowOptions options;
	if (HGF::WindowOptions::FromFile("data/window.json", options))
		if (!mWindow.Reinitialize(options))
			return -1;
	mWindow.SetClearColor(HGF::Color(0.4f, 0.45f, 0.5f));
	mWindow.PrintInfo();

	mFrameCount = 0;
	mPreviousTicks = 0;
	mCurrentTicks = SDL_GetTicks();

	GameplayScreen* screen = new GameplayScreen(&mScreenManager);
	mScreenManager.Push(screen);

	return 0;
}

int JumpShootGame::Finalize()
{
	if (SAGE::Game::Finalize() < 0)
		return -1;

	return 0;
}

int JumpShootGame::Update(float pDeltaTime)
{
	mFrameCount++;
	mCurrentTicks = SDL_GetTicks();
	if (mCurrentTicks > mPreviousTicks + 1000)
	{
		std::cout << "FPS: " << mFrameCount << " | Delta: " << pDeltaTime << " | SB Calls / sec: " << mSBDrawCalls / mFrameCount << " | GB Calls / sec: " << mGBDrawCalls / mFrameCount << std::endl;
		
		mPreviousTicks = mCurrentTicks;
		mFrameCount = 0;
		mSBDrawCalls = 0;
		mGBDrawCalls = 0;
	}

	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Escape))
		Quit();

	mScreenManager.Update(pDeltaTime);

	return 0;
}

int JumpShootGame::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	mScreenManager.Render(pSpriteBatch);

	mSBDrawCalls += pSpriteBatch.GetDrawCallCount();

	return 0;
}

int JumpShootGame::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	if (Globals::IsDebugDrawOn)
	{
		mScreenManager.Render(pGeometryBatch);
	}

	mGBDrawCalls += pGeometryBatch.GetDrawCallCount();

	return 0;
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	JumpShootGame game;
	game.Start();

	return 0;
}