#include "Game.hpp"

#include <SDL2\SDL_opengl.h>

#include <iostream>

Game::Game()
{
	mIsRunning = true;
	
	mWindowWidth = 1024;
	mWindowHeight = 768;

	mCamera.Position = HGF::Vector2(0.0f, 0.0f);
	mCamera.Rotation = 0.0f;
	mCamera.Zoom = 2.0f;
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

	mPlayer.TextureID = mRenderer.LoadTexture("data/img/megamanx.png");
	if (mPlayer.TextureID < 0)
		return -1;

	mMap.Tileset.TextureID = mRenderer.LoadTexture("data/img/tileset2.png");
	if (mMap.Tileset.TextureID < 0)
		return -1;

	return 0;
}

int Game::Finalize()
{
	// delete textures
	glDeleteTextures(1, &mPlayer.TextureID);
	glDeleteTextures(1, &mMap.Tileset.TextureID);

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
	if (mKeys[SDLK_RETURN])
	{
		//GenerateMap(map);
	}
	if (mKeys[SDLK_BACKSPACE])
	{
		mPlayer.Position = HGF::Vector2(100.0f, 100.0f);
	}

	// handle input
	if (mKeys[SDLK_ESCAPE])
	{
		mIsRunning = false;
	}
	if (mKeys[SDLK_SPACE] && mPlayer.IsGrounded && !mPlayer.CollisionMarkers[Player::MRK_HEAD_LEFT].IsTouching && !mPlayer.CollisionMarkers[Player::MRK_HEAD_RIGHT].IsTouching)
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

	mWorld.HandleWorldCollisionViaPoints(mPlayer, mMap);

	return 0;
}

int Game::Render()
{
	// focus camera
	mCamera.Position.X = mWindowWidth / 4.0f - mPlayer.Position.X - mPlayer.Dimensions.X / 2.0f;
	mCamera.Position.Y = mWindowHeight / 4.0f - mPlayer.Position.Y - mPlayer.Dimensions.Y / 2.0f;

	// clear
	glClearColor(150.0f / 255.0f, 175.0f / 255.0f, 225.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// render loop
	if (mRenderer.RenderMap(mCamera, mMap) < 0)
		return -1;
	if (mRenderer.RenderPlayer(mCamera, mPlayer) < 0)
		return -1;

	// flip
	SDL_GL_SwapWindow(mWindow);

	return 0;
}