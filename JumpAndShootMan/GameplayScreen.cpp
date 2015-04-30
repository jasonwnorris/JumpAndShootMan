// GameplayScreen.cpp

#include "Globals.hpp"
// HGF Includes
#include <HGF\Keyboard.hpp>
// Project Includes
#include "GameplayScreen.hpp"

GameplayScreen::GameplayScreen()
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
	mWorld.Initialize();

	// Create and load the player.
	mPlayer = mWorld.Create<Player>();
	if (!mPlayer->Load("data/img/player.png"))
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
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Backspace))
	{
		mPlayer->Position = HGF::Vector2(100.0f, 100.0f);
		mPlayer->IsGrounded = false;
	}
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::P))
	{
		Globals::IsDebugDrawOn = !Globals::IsDebugDrawOn;
	}
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::L))
	{
		mPlayer->IsDebugFly = !mPlayer->IsDebugFly;
		mPlayer->Velocity = HGF::Vector2::Zero;
		mPlayer->Acceleration = HGF::Vector2::Zero;
	}

	// handle input
	if (mPlayer->IsDebugFly)
	{
		float mult = 50.0f;
		if (HGF::Keyboard::IsKeyDown(HGF::Key::Up))
		{
			mPlayer->Position.Y -= mPlayer->MovementSpeed * mult;
		}
		if (HGF::Keyboard::IsKeyDown(HGF::Key::Down))
		{
			mPlayer->Position.Y += mPlayer->MovementSpeed * mult;
		}
		if (HGF::Keyboard::IsKeyDown(HGF::Key::Left))
		{
			mPlayer->Position.X -= mPlayer->MovementSpeed * mult;
			mPlayer->IsFacingLeft = true;
		}
		if (HGF::Keyboard::IsKeyDown(HGF::Key::Right))
		{
			mPlayer->Position.X += mPlayer->MovementSpeed * mult;
			mPlayer->IsFacingLeft = false;
		}
	}
	else
	{
		if (HGF::Keyboard::IsKeyPressed(HGF::Key::Z) && mPlayer->IsGrounded)
		{
			mPlayer->Velocity.Y = 0.0f;
			mPlayer->Acceleration.Y -= mPlayer->JumpingSpeed;
			mPlayer->IsGrounded = false;
		}
		if (HGF::Keyboard::IsKeyPressed(HGF::Key::X))
		{
			mPlayer->Fire();
		}
		if (HGF::Keyboard::IsKeyDown(HGF::Key::Left))
		{
			mPlayer->Acceleration.X -= mPlayer->MovementSpeed;
			mPlayer->IsFacingLeft = true;
		}
		if (HGF::Keyboard::IsKeyDown(HGF::Key::Right))
		{
			mPlayer->Acceleration.X += mPlayer->MovementSpeed;
			mPlayer->IsFacingLeft = false;
		}

		// update movement
		if (!mPlayer->IsGrounded)
		{
			mPlayer->Acceleration.Y += mPlayer->Gravity;
		}
		mPlayer->Velocity += mPlayer->Acceleration;
		mPlayer->Position += mPlayer->Velocity;
		mPlayer->Velocity.X *= 0.75f;
		mPlayer->Acceleration *= 0.75f;
	}

	mWorld.Update(pDeltaTime);

	mCamera.SetPosition(mPlayer->Position);

	return 0;
}

int GameplayScreen::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	pSpriteBatch.Begin(mSpriteEffect, mCamera, SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended, SAGE::RasterizerState::None);
	mWorld.Render(pSpriteBatch);
	pSpriteBatch.DrawString(mSpriteFont, "Jump 'n' Shoot Man!", HGF::Vector2::Zero, HGF::Color::White, HGF::Vector2::Zero, 0.0f, HGF::Vector2::One, SAGE::Orientation::None);
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