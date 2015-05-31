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

	m_SBDrawCalls = 0;
	m_GBDrawCalls = 0;
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

	m_FrameCount = 0;
	m_PreviousTicks = 0;
	m_CurrentTicks = SDL_GetTicks();

	GameplayScreen* screen = new GameplayScreen(&m_ScreenManager);
	m_ScreenManager.Push(screen);

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
	m_FrameCount++;
	m_CurrentTicks = SDL_GetTicks();
	if (m_CurrentTicks > m_PreviousTicks + 1000)
	{
		std::cout << "FPS: " << m_FrameCount << " | Delta: " << pDeltaTime << " | SB Calls / sec: " << m_SBDrawCalls / m_FrameCount << " | GB Calls / sec: " << m_GBDrawCalls / m_FrameCount << std::endl;
		
		m_PreviousTicks = m_CurrentTicks;
		m_FrameCount = 0;
		m_SBDrawCalls = 0;
		m_GBDrawCalls = 0;
	}

	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Escape))
		Quit();

	m_ScreenManager.Update(pDeltaTime);

	return 0;
}

int JumpShootGame::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	m_ScreenManager.Render(pSpriteBatch);

	m_SBDrawCalls += pSpriteBatch.GetDrawCallCount();

	return 0;
}

int JumpShootGame::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	if (Globals::IsDebugDrawOn)
	{
		m_ScreenManager.Render(pGeometryBatch);
	}

	m_GBDrawCalls += pGeometryBatch.GetDrawCallCount();

	return 0;
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	JumpShootGame game;
	game.Start();

	return 0;
}