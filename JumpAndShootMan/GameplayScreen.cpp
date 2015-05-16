// GameplayScreen.cpp

#include "Globals.hpp"
// HGF Includes
#include <HGF\Keyboard.hpp>
// Project Includes
#include "GameplayScreen.hpp"

GameplayScreen::GameplayScreen(SAGE::ScreenManager* pManager) : SAGE::Screen(pManager)
{
}

GameplayScreen::~GameplayScreen()
{
	mPlayer = nullptr;
}

int GameplayScreen::Initialize()
{
	// Set up the camera.
	mCamera.SetDimensions(Globals::ScreenWidth, Globals::ScreenHeight);
	mCamera.SetScale(HGF::Vector2::One * 2.0f);

	// Initialization for geometry batching.
	if (!mGeometryEffect.Create(HGF::Effect::BasicType::PositionColor))
		return -1;

	// Initialization for sprite batching.
	if (!mSpriteEffect.Create(HGF::Effect::BasicType::PositionColorTexture))
		return -1;

	// Load the sprite font.
	if (!mSpriteFont.Load("data/img/font.png", 26.0f, 16.0f))
		return -1;

	// Initialize the world.
	if (mWorld.Initialize() < 0)
		return -1;

	// Create and load the player.
	mPlayer = mWorld.Create<Player>();
	if (!mPlayer->Load("data/img/player_sheet.png"))
		return -1;

	return 0;
}

int GameplayScreen::Finalize()
{
	// Finalize the world.
	mWorld.Finalize();

	return 0;
}

int GameplayScreen::Update(float pDeltaTime)
{
	// debug
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::P))
	{
		Globals::IsDebugDrawOn = !Globals::IsDebugDrawOn;
	}

	mWorld.Update(pDeltaTime);

	mCamera.SetPosition(mPlayer->Position);

	mData = "On Ground: ";
	if (mPlayer->IsGrounded)
		mData += "true";
	else
		mData += "false";
	mData += "\nIs Jumping: ";
	if (mPlayer->IsJumping)
		mData += "true";
	else
		mData += "false";

	return 0;
}

int GameplayScreen::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	pSpriteBatch.Begin(mSpriteEffect, mCamera, SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended);
	mWorld.Render(pSpriteBatch);
	pSpriteBatch.End();

	pSpriteBatch.Begin(mSpriteEffect, SAGE::Camera2D::DefaultCamera, SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended);
	pSpriteBatch.DrawString(mSpriteFont, mData, HGF::Vector2(-620.0f, -340.0f), HGF::Color::White);
	pSpriteBatch.End();

	return 0;
}

int GameplayScreen::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	pGeometryBatch.Begin(mGeometryEffect, mCamera);
	mWorld.Render(pGeometryBatch);
	pGeometryBatch.End();

	return 0;
}