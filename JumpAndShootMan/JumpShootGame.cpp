// JumpShootGame.cpp

// HGF Includes
#include <HGF\Events.hpp>
#include <HGF\Keyboard.hpp>
// GL Includes
#include <GL\glew.h>
// SDL Includes
#include <SDL2\SDL_opengl.h>
// Project Includes
#include "JumpShootGame.hpp"
#include "Globals.hpp"
// STL Includes
#include <iostream>

JumpShootGame::JumpShootGame()
{
	Globals::IsDebugDrawOn = false;
	
	mWindowWidth = 1024;
	mWindowHeight = 768;

	mCamera.Position = HGF::Vector2(0.0f, 0.0f);
	mCamera.Rotation = 0.0f;
	mCamera.Zoom = 1.0f;
}

JumpShootGame::~JumpShootGame()
{
}

int JumpShootGame::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return -1;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	mWindow = SDL_CreateWindow("JUMP AND SHOOT MAN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth, mWindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (mWindow == NULL)
		return -1;

	mContext = SDL_GL_CreateContext(mWindow);
	if (mContext == NULL)
		return -1;

	if (SDL_GL_SetSwapInterval(1) < 0)
		return -1;

	if (!mSpriteBatch.Initialize())
		return -1;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, mWindowWidth, mWindowHeight, 0, -1, 1);

	glViewport(0, 0, mWindowWidth, mWindowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mFrameCount = 0;
	mPreviousTicks = 0;
	mCurrentTicks = SDL_GetTicks();

	if (!mMap.Load("data/maps/first_redux.txt"))
		return -1;

	if (!mPlayer.Load("data/img/player.png"))
		return -1;

	return 0;
}

int JumpShootGame::Finalize()
{
	if (!mSpriteBatch.Finalize())
		return -1;

	// clean up
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	return 0;
}

int JumpShootGame::Update(float pDeltaTime)
{
	mFrameCount++;
	mCurrentTicks = SDL_GetTicks();
	if (mCurrentTicks > mPreviousTicks + 1000)
	{
		std::cout << "FPS: " << mFrameCount << " | Delta: "  << pDeltaTime << std::endl;
		mPreviousTicks = mCurrentTicks;
		mFrameCount = 0;
	}

	// debug
	if (HGF::Keyboard::IsKeyPressed(SDLK_BACKSPACE))
	{
		mPlayer.Position = HGF::Vector2(100.0f, 100.0f);
		mPlayer.IsGrounded = false;
	}
	if (HGF::Keyboard::IsKeyPressed(SDLK_p))
	{
		Globals::IsDebugDrawOn = !Globals::IsDebugDrawOn;
	}
	if (HGF::Keyboard::IsKeyPressed(SDLK_l))
	{
		mPlayer.IsDebugFly = !mPlayer.IsDebugFly;
		if (mPlayer.IsDebugFly)
			mPlayer.MovementSpeed = 2.685f;
		else
			mPlayer.MovementSpeed = 0.185f;
	}

	// exit
	if (HGF::Keyboard::IsKeyPressed(SDLK_ESCAPE))
	{
		mRunning = false;
	}

	// handle input
	if (mPlayer.IsDebugFly)
	{
		if (HGF::Keyboard::IsKeyDown(SDLK_UP))
		{
			mPlayer.Position.Y -= mPlayer.MovementSpeed;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_DOWN))
		{
			mPlayer.Position.Y += mPlayer.MovementSpeed;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_LEFT))
		{
			mPlayer.Position.X -= mPlayer.MovementSpeed;
			mPlayer.IsFacingLeft = true;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_RIGHT))
		{
			mPlayer.Position.X += mPlayer.MovementSpeed;
			mPlayer.IsFacingLeft = false;
		}
	}
	else
	{
		if (HGF::Keyboard::IsKeyPressed(SDLK_SPACE) && mPlayer.IsGrounded)
		{
			mPlayer.Velocity.Y = 0.0f;
			mPlayer.Acceleration.Y -= mPlayer.JumpingSpeed;
			mPlayer.IsGrounded = false;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_LEFT))
		{
			mPlayer.Acceleration.X -= mPlayer.MovementSpeed;
			mPlayer.IsFacingLeft = true;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_RIGHT))
		{
			mPlayer.Acceleration.X += mPlayer.MovementSpeed;
			mPlayer.IsFacingLeft = false;
		}

		// update movement
		if (!mPlayer.IsGrounded)
		{
			mPlayer.Acceleration.Y += mPlayer.Gravity;
		}
		mPlayer.Velocity += mPlayer.Acceleration;
		mPlayer.Position += mPlayer.Velocity;
		mPlayer.Velocity.X *= 0.75f;
		mPlayer.Acceleration *= 0.75f;
	}

	UpdateRaycast();

	return 0;
}

int JumpShootGame::Render()
{
	// clear
	glClearColor(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// focus camera
	mCamera.Position.X = mWindowWidth / 2.0f - (mPlayer.Position.X + mPlayer.Dimensions.X / 2.0f) * mCamera.Zoom;
	mCamera.Position.Y = mWindowHeight / 2.0f - (mPlayer.Position.Y + mPlayer.Dimensions.Y / 2.0f) * mCamera.Zoom;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(mCamera.Position.X, mCamera.Position.Y, 0.0f);
	glRotatef(mCamera.Rotation, 0.0f, 0.0f, 1.0f);
	glScalef(mCamera.Zoom, mCamera.Zoom, 0.0f);
	
	// render loop
	if (!mMap.Render(mRenderer))
		return -1;
	if (!mPlayer.Render(mRenderer))
		return -1;

	// flip
	SDL_GL_SwapWindow(mWindow);

	return 0;
}

void JumpShootGame::UpdateRaycast()
{
	for (int i = 0; i < Player::MaxRayCount; i += 3)
	{
		mMap.Raycast(mPlayer.Position + mPlayer.RaycastInfos[i].Position, mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i].HasInterest, mPlayer.RaycastHits[i]);

		if (mPlayer.RaycastHits[i].Distance < mPlayer.RaycastInfos[i].Threshold)
		{
			if (mPlayer.RaycastHits[i].Distance < mPlayer.RaycastInfos[i].Distance || (mPlayer.RaycastInfos[i].Direction == Direction::Down && (mPlayer.IsGrounded && !mPlayer.IsDebugFly)))
			{
				UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i].Distance - mPlayer.RaycastHits[i].Distance);
			}
		}
		else
		{
			bool hit[2];
			for (int j = 0; j < 2; ++j)
			{
				mMap.Raycast(mPlayer.Position + mPlayer.RaycastInfos[i + j + 1].Position, mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + j + 1].HasInterest, mPlayer.RaycastHits[i + j + 1]);
				hit[j] = mPlayer.RaycastHits[i + j + 1].Distance < mPlayer.RaycastInfos[i + j + 1].Distance;
			}

			if (hit[0] && hit[1])
			{
				if (mPlayer.RaycastHits[i + 1].Distance > mPlayer.RaycastHits[i + 2].Distance)
				{
					UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 1].Distance - mPlayer.RaycastHits[i + 1].Distance);
				}
				else
				{
					UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 2].Distance - mPlayer.RaycastHits[i + 2].Distance);
				}
			}
			else if (hit[0])
			{
				UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 1].Distance - mPlayer.RaycastHits[i + 1].Distance);
			}
			else if (hit[1])
			{
				UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 2].Distance - mPlayer.RaycastHits[i + 2].Distance);
			} 
			else if (mPlayer.RaycastInfos[i].Direction == Direction::Down)
			{
				mPlayer.IsGrounded = false;
			}
		}
	}
}

void JumpShootGame::UpdateAdjustment(Direction pDirection, float pDistance)
{
	switch (pDirection)
	{
		case Direction::Up:
			mPlayer.Position.Y += pDistance;
			mPlayer.Velocity.Y = 0.0f;
			mPlayer.Acceleration.Y = 0.0f;
			break;
		case Direction::Down:
			mPlayer.Position.Y -= pDistance;
			mPlayer.Velocity.Y = 0.0f;
			mPlayer.Acceleration.Y = 0.0f;
			mPlayer.IsGrounded = true;
			break;
		case Direction::Left:
			mPlayer.Position.X += pDistance;
			mPlayer.Velocity.X = 0.0f;
			mPlayer.Acceleration.X = 0.0f;
			break;
		case Direction::Right:
			mPlayer.Position.X -= pDistance;
			mPlayer.Velocity.X = 0.0f;
			mPlayer.Acceleration.X = 0.0f;
			break;
	}
}