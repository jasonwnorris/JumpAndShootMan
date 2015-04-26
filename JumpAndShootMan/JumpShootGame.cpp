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
	
	mWindowWidth = 1280;
	mWindowHeight = 720;
}

JumpShootGame::~JumpShootGame()
{
}

int JumpShootGame::Initialize()
{
	// Hook the event for clicking window X.
	HGF::Events::OnQuit.Add([&]{ mRunning = false; });

	// Initialize the window.
	HGF::WindowOptions options;
	options.Title = "Jump 'n' Shoot Man";
	options.Width = mWindowWidth;
	options.Height = mWindowHeight;
	options.Mode = HGF::WindowMode::Windowed;
	//options.VerticalSync = false;
	if (!mWindow.Initialize(options))
		return -1;
	mWindow.SetClearColor(HGF::Color(0.4f, 0.45f, 0.5f));
	mWindow.PrintInfo();

	// Set up the camera.
	mCamera.SetDimensions(mWindowWidth, mWindowHeight);

	// Initialization for geometry batching.
	if (!mGeometryBatch.Initialize())
		return -1;
	if (!mGeometryEffect.Create(HGF::Effect::BasicType::PositionColor))
		return -1;

	// Initialization for sprite batching.
	if (!mSpriteBatch.Initialize())
		return -1;
	if (!mSpriteEffect.Create(HGF::Effect::BasicType::PositionColorTexture))
		return -1;

	// Load the sprite font.
	if (!mSpriteFont.Load("data/img/font.png", 26.0f, 16.0f))
		return -1;

	// FPS tracking.
	mFrameCount = 0;
	mPreviousTicks = 0;
	mCurrentTicks = SDL_GetTicks();

	// Load the map.
	if (!mTiledMap.Load("data/maps/first.txt"))
		return -1;

	// Create and load the player.
	mPlayer = mEntityManager.Create<Player>();
	if (!mPlayer->Load("data/img/player.png"))
		return -1;

	return 0;
}

int JumpShootGame::Finalize()
{
	if (!mGeometryBatch.Finalize())
		return -1;

	if (!mSpriteBatch.Finalize())
		return -1;

	if (!mWindow.Finalize())
		return -1;

	return 0;
}

int JumpShootGame::Update(float pDeltaTime)
{
	mFrameCount++;
	mCurrentTicks = SDL_GetTicks();
	if (mCurrentTicks > mPreviousTicks + 1000)
	{
		std::cout << "FPS: " << mFrameCount << " | Delta: "  << pDeltaTime << " | SB Calls: " << mSpriteBatch.GetDrawCallCount() << " | GB Calls: " << mGeometryBatch.GetDrawCallCount() << std::endl;
		
		mPreviousTicks = mCurrentTicks;
		mFrameCount = 0;
	}

	// exit
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::Escape))
	{
		mRunning = false;
	}

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

	UpdateRaycast();

	mEntityManager.Update(pDeltaTime);

	mCamera.SetPosition(mPlayer->Position);

	return 0;
}

int JumpShootGame::Render()
{
	mWindow.Clear();

	mSpriteBatch.Begin(mSpriteEffect, mCamera, SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended, SAGE::RasterizerState::None);
	mTiledMap.Render(mSpriteBatch);
	mSpriteBatch.DrawString(mSpriteFont, "Jump 'n' Shoot Man!", HGF::Vector2::Zero, HGF::Color::White, HGF::Vector2::Zero, 0.0f, HGF::Vector2::One, SAGE::Orientation::None);
	mEntityManager.Render(mSpriteBatch);
	mSpriteBatch.End();

	if (Globals::IsDebugDrawOn)
	{
		mGeometryBatch.Begin(mGeometryEffect, mCamera);
		mTiledMap.RenderDebug(mGeometryBatch);
		mPlayer->RenderDebug(mGeometryBatch);
		mGeometryBatch.End();
	}

	mWindow.Flip();

	return 0;
}

void JumpShootGame::UpdateRaycast()
{
	for (int i = 0; i < Player::MaxRayCount; i += 3)
	{
		mTiledMap.Raycast(mPlayer->Position + mPlayer->RaycastInfos[i].Position, mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i].HasInterest, mPlayer->RaycastHits[i]);

		if (mPlayer->RaycastHits[i].Distance < mPlayer->RaycastInfos[i].Threshold)
		{
			if (mPlayer->RaycastHits[i].Distance < mPlayer->RaycastInfos[i].Distance || (mPlayer->RaycastInfos[i].Direction == Direction::Down && (mPlayer->IsGrounded && !mPlayer->IsDebugFly)))
			{
				UpdateAdjustment(mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i].Distance - mPlayer->RaycastHits[i].Distance);
			}
		}
		else
		{
			bool hit[2];
			for (int j = 0; j < 2; ++j)
			{
				mTiledMap.Raycast(mPlayer->Position + mPlayer->RaycastInfos[i + j + 1].Position, mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i + j + 1].HasInterest, mPlayer->RaycastHits[i + j + 1]);
				hit[j] = mPlayer->RaycastHits[i + j + 1].Distance < mPlayer->RaycastInfos[i + j + 1].Distance;
			}

			if (hit[0] && hit[1])
			{
				if (mPlayer->RaycastHits[i + 1].Distance > mPlayer->RaycastHits[i + 2].Distance)
				{
					UpdateAdjustment(mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i + 1].Distance - mPlayer->RaycastHits[i + 1].Distance);
				}
				else
				{
					UpdateAdjustment(mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i + 2].Distance - mPlayer->RaycastHits[i + 2].Distance);
				}
			}
			else if (hit[0])
			{
				UpdateAdjustment(mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i + 1].Distance - mPlayer->RaycastHits[i + 1].Distance);
			}
			else if (hit[1])
			{
				UpdateAdjustment(mPlayer->RaycastInfos[i].Direction, mPlayer->RaycastInfos[i + 2].Distance - mPlayer->RaycastHits[i + 2].Distance);
			} 
			else if (mPlayer->RaycastInfos[i].Direction == Direction::Down)
			{
				mPlayer->IsGrounded = false;
			}
		}
	}
}

void JumpShootGame::UpdateAdjustment(Direction pDirection, float pDistance)
{
	switch (pDirection)
	{
		case Direction::Up:
			mPlayer->Position.Y += pDistance;
			mPlayer->Velocity.Y = 0.0f;
			mPlayer->Acceleration.Y = 0.0f;
			break;
		case Direction::Down:
			mPlayer->Position.Y -= pDistance;
			mPlayer->Velocity.Y = 0.0f;
			mPlayer->Acceleration.Y = 0.0f;
			mPlayer->IsGrounded = true;
			break;
		case Direction::Left:
			mPlayer->Position.X += pDistance;
			mPlayer->Velocity.X = 0.0f;
			mPlayer->Acceleration.X = 0.0f;
			break;
		case Direction::Right:
			mPlayer->Position.X -= pDistance;
			mPlayer->Velocity.X = 0.0f;
			mPlayer->Acceleration.X = 0.0f;
			break;
	}
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	JumpShootGame game;
	game.Run();

	return 0;
}