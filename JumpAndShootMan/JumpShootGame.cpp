// JumpShootGame.cpp

#include "Globals.hpp"
// HGF Includes
#include <HGF\Events.hpp>
#include <HGF\Keyboard.hpp>
// SDL Includes
#include <SDL2\SDL_opengl.h>
// Project Includes
#include "JumpShootGame.hpp"
// STL Includes
#include <iostream>

JumpShootGame::JumpShootGame()
{
	Globals::IsDebugDrawOn = false;
	
	mWindowWidth = 1024;
	mWindowHeight = 768;
}

JumpShootGame::~JumpShootGame()
{
}

int JumpShootGame::Initialize()
{
	HGF::Events::OnQuit.Add([&]{ mRunning = false; });

	if (!mWindow.Initialize("JUMP AND SHOOT MAN", mWindowWidth, mWindowHeight))
		return -1;
	mWindow.SetClearColor(HGF::Color(0.4f, 0.45f, 0.5f));
	mWindow.SetVerticalSync(false);
	mWindow.PrintInfo();

	if (!mGeometryBatch.Initialize())
		return -1;

	if (!mGeometryEffect.Initialize())
		return -1;
	if (!mGeometryEffect.Attach("data/shaders/positioncolor.vs.glsl", HGF::Effect::ShaderType::Vertex))
		return -1;
	if (!mGeometryEffect.Attach("data/shaders/positioncolor.fs.glsl", HGF::Effect::ShaderType::Fragment))
		return -1;
	if (!mGeometryEffect.Link())
		return -1;
	mGeometryEffect.Use();

	if (!mSpriteBatch.Initialize())
		return -1;

	if (!mSpriteEffect.Initialize())
		return -1;
	if (!mSpriteEffect.Attach("data/shaders/positioncolortexture.vs.glsl", HGF::Effect::ShaderType::Vertex))
		return -1;
	if (!mSpriteEffect.Attach("data/shaders/positioncolortexture.fs.glsl", HGF::Effect::ShaderType::Fragment))
		return -1;
	if (!mSpriteEffect.Link())
		return -1;
	mSpriteEffect.Use();
	mSpriteEffect.SetUniform("uTextureSampler", 0);

	mFrameCount = 0;
	mPreviousTicks = 0;
	mCurrentTicks = SDL_GetTicks();

	if (!mMap.Load("data/maps/first.txt"))
		return -1;

	if (!mPlayer.Load("data/img/player.png"))
		return -1;

	return 0;
}

int JumpShootGame::Finalize()
{
	if (!mGeometryEffect.Finalize())
		return -1;

	if (!mGeometryBatch.Finalize())
		return -1;

	if (!mSpriteEffect.Finalize())
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
		std::cout << "FPS: " << mFrameCount << " | Delta: "  << pDeltaTime << std::endl;
		mPreviousTicks = mCurrentTicks;
		mFrameCount = 0;
	}

	// exit
	if (HGF::Keyboard::IsKeyPressed(SDLK_ESCAPE))
	{
		mRunning = false;
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
		mPlayer.Velocity = HGF::Vector2::Zero;
		mPlayer.Acceleration = HGF::Vector2::Zero;
	}

	float dt = pDeltaTime * 25.0f;

	// handle input
	if (mPlayer.IsDebugFly)
	{
		float speed = 50.0f;
		if (HGF::Keyboard::IsKeyDown(SDLK_UP))
		{
			mPlayer.Position.Y -= mPlayer.MovementSpeed * dt * speed;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_DOWN))
		{
			mPlayer.Position.Y += mPlayer.MovementSpeed * dt * speed;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_LEFT))
		{
			mPlayer.Position.X -= mPlayer.MovementSpeed * dt * speed;
			mPlayer.IsFacingLeft = true;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_RIGHT))
		{
			mPlayer.Position.X += mPlayer.MovementSpeed * dt * speed;
			mPlayer.IsFacingLeft = false;
		}
	}
	else
	{
		if (HGF::Keyboard::IsKeyPressed(SDLK_SPACE) && mPlayer.IsGrounded)
		{
			mPlayer.Velocity.Y = 0.0f;
			mPlayer.Acceleration.Y -= mPlayer.JumpingSpeed * dt;
			mPlayer.IsGrounded = false;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_LEFT))
		{
			mPlayer.Acceleration.X -= mPlayer.MovementSpeed * dt;
			mPlayer.IsFacingLeft = true;
		}
		if (HGF::Keyboard::IsKeyDown(SDLK_RIGHT))
		{
			mPlayer.Acceleration.X += mPlayer.MovementSpeed * dt;
			mPlayer.IsFacingLeft = false;
		}

		// update movement
		if (!mPlayer.IsGrounded)
		{
			mPlayer.Acceleration.Y += mPlayer.Gravity * dt;
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
	glm::mat4 projectionMatrix(glm::ortho(0.0f, (float)mWindowWidth, (float)mWindowHeight, 0.0f, 1.0f, -1.0f));

	glm::mat4 modelViewMatrix;
	modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3((float)mWindowWidth / 2.0f, (float)mWindowHeight / 2.0f, 0.0f));
	modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(-mPlayer.Position.X, -mPlayer.Position.Y, 0.0f));

	mWindow.Clear();

	mSpriteEffect.SetProjection(projectionMatrix);
	mSpriteEffect.SetModelView(modelViewMatrix);
	mSpriteEffect.Use();

	mSpriteBatch.Begin(SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended);
	mMap.Render(mSpriteBatch);
	mPlayer.Render(mSpriteBatch);
	mSpriteBatch.End();

	if (Globals::IsDebugDrawOn)
	{
		mGeometryEffect.SetProjection(projectionMatrix);
		mGeometryEffect.SetModelView(modelViewMatrix);
		mGeometryEffect.Use();

		mGeometryBatch.Begin();
		mMap.RenderDebug(mGeometryBatch);
		mPlayer.RenderDebug(mGeometryBatch);
		mGeometryBatch.End();
	}

	mWindow.Flip();

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