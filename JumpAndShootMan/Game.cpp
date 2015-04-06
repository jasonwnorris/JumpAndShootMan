#include "Game.hpp"
#include "Globals.hpp"

#include <SDL2\SDL_opengl.h>

#include <iostream>

Game::Game()
{
	Globals::IsDebugDrawOn = false;

	mIsRunning = true;
	
	mWindowWidth = 1024;
	mWindowHeight = 768;

	mCamera.Position = HGF::Vector2(0.0f, 0.0f);
	mCamera.Rotation = 0.0f;
	mCamera.Zoom = 1.0f;
}

Game::~Game()
{
}

int Game::Run()
{
	if (Initialize() < 0)
		return -1;

	while (mIsRunning)
	{
		mTimer.Update();

		if (HandleInput() < 0)
			return -1;

		if (Update(mTimer.GetDeltaTime()) < 0)
			return -1;

		if (Render() < 0)
			return -1;

		SDL_Delay(10);
	}

	if (Finalize() < 0)
		return -1;

	return 0;
}

int Game::Initialize()
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

int Game::Finalize()
{
	// clean up
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	return 0;
}

int Game::HandleInput()
{
	// event handling
	while (SDL_PollEvent(&mEvent))
	{
		switch (mEvent.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
			case SDL_KEYDOWN:
				mKeys[mEvent.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				mKeys[mEvent.key.keysym.sym] = false;
				break;
			default:
				break;
		}
	}

	// debug
	if (mKeys[SDLK_BACKSPACE])
	{
		mPlayer.Position = HGF::Vector2(100.0f, 100.0f);
	}
	if (mKeys[SDLK_p])
	{
		Globals::IsDebugDrawOn = !Globals::IsDebugDrawOn;
	}

	// exit
	if (mKeys[SDLK_ESCAPE])
	{
		mIsRunning = false;
	}

	/*
	// movement
	if (mKeys[SDLK_UP])
	{
		mPlayer.Position.Y -= mPlayer.MovementSpeed;
	}
	if (mKeys[SDLK_DOWN])
	{
		mPlayer.Position.Y += mPlayer.MovementSpeed;
	}
	if (mKeys[SDLK_LEFT])
	{
		mPlayer.Position.X -= mPlayer.MovementSpeed;
		mPlayer.IsFacingLeft = true;
	}
	if (mKeys[SDLK_RIGHT])
	{
		mPlayer.Position.X += mPlayer.MovementSpeed;
		mPlayer.IsFacingLeft = false;
	}
	*/

	// handle input
	if (mKeys[SDLK_SPACE] && mPlayer.IsGrounded)
	{
		mPlayer.Velocity.Y = 0.0f;
		mPlayer.Acceleration.Y -= mPlayer.JumpingSpeed;
		mPlayer.IsGrounded = false;
	}
	if (mKeys[SDLK_LEFT])
	{
		mPlayer.Acceleration.X -= mPlayer.MovementSpeed;
		mPlayer.IsFacingLeft = true;
	}
	if (mKeys[SDLK_RIGHT])
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

	return 0;
}

int Game::Update(float pDeltaTime)
{
	mFrameCount++;
	mCurrentTicks = SDL_GetTicks();
	if (mCurrentTicks > mPreviousTicks + 1000)
	{
		std::cout << "FPS: " << mFrameCount << std::endl;
		mPreviousTicks = mCurrentTicks;
		mFrameCount = 0;
	}

	UpdateRaycast();

	return 0;
}

int Game::Render()
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

void Game::UpdateRaycast()
{
	for (int i = 0; i < Player::MaxRayCount; i += 3)
	{
		mMap.Raycast(mPlayer.Position + mPlayer.RaycastInfos[i].Position, mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i].HasInterest, mPlayer.RaycastHits[i]);

		if (mPlayer.RaycastHits[i].Distance < mPlayer.RaycastInfos[i].Threshold)
		{
			if (mPlayer.RaycastHits[i].Distance < mPlayer.RaycastInfos[i].Distance || (mPlayer.RaycastInfos[i].Direction == Direction::Down && mPlayer.IsGrounded))
			{
				UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i].Distance - mPlayer.RaycastHits[i].Distance);
			}
		}
		else
		{
			mMap.Raycast(mPlayer.Position + mPlayer.RaycastInfos[i + 1].Position, mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 1].HasInterest, mPlayer.RaycastHits[i + 1]);
			mMap.Raycast(mPlayer.Position + mPlayer.RaycastInfos[i + 2].Position, mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 2].HasInterest, mPlayer.RaycastHits[i + 2]);
		
			if (mPlayer.RaycastHits[i + 1].Distance < mPlayer.RaycastInfos[i + 1].Distance && mPlayer.RaycastHits[i + 2].Distance < mPlayer.RaycastInfos[i + 2].Distance)
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
			else
			{
				if (mPlayer.RaycastHits[i + 1].Distance < mPlayer.RaycastInfos[i + 1].Distance)
				{
					UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 1].Distance - mPlayer.RaycastHits[i + 1].Distance);
				}
				if (mPlayer.RaycastHits[i + 2].Distance < mPlayer.RaycastInfos[i + 2].Distance)
				{
					UpdateAdjustment(mPlayer.RaycastInfos[i].Direction, mPlayer.RaycastInfos[i + 2].Distance - mPlayer.RaycastHits[i + 2].Distance);
				}
			}
		}
	}
}

void Game::UpdateAdjustment(Direction pDirection, float pDistance)
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