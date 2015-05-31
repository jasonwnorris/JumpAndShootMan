// GameplayScreen.cpp

#include "Globals.hpp"
// HGF Includes
#include <HGF\Keyboard.hpp>
// Project Includes
#include "GameplayScreen.hpp"
#include "TransformComponent.hpp"

GameplayScreen::GameplayScreen(SAGE::ScreenManager* p_Manager) : SAGE::Screen(p_Manager)
{
}

GameplayScreen::~GameplayScreen()
{
	m_Player = nullptr;
}

int GameplayScreen::Initialize()
{
	// Set up the camera.
	m_Camera.SetDimensions(Globals::ScreenWidth, Globals::ScreenHeight);
	m_Camera.SetScale(HGF::Vector2::One * 2.0f);

	// Initialization for geometry batching.
	if (!m_GeometryEffect.Create(HGF::Effect::BasicType::PositionColor))
	{
		return -1;
	}

	// Initialization for sprite batching.
	if (!m_SpriteEffect.Create(HGF::Effect::BasicType::PositionColorTexture))
	{
		return -1;
	}

	// Load the sprite font.
	if (!m_SpriteFont.Load("data/img/font.png", 26.0f, 16.0f))
	{
		return -1;
	}

	// Initialize the world.
	if (m_World.Initialize() < 0)
	{
		return -1;
	}

	// Create and load the player.
	m_Player = m_World.CreateEntity<Player>();

	return 0;
}

int GameplayScreen::Finalize()
{
	// Finalize the world.
	m_World.Finalize();

	return 0;
}

int GameplayScreen::Update(float pDeltaTime)
{
	// debug
	if (HGF::Keyboard::IsKeyPressed(HGF::Key::P))
	{
		Globals::IsDebugDrawOn = !Globals::IsDebugDrawOn;
	}

	m_World.Update(pDeltaTime);

	m_Data = "(Z) Jump\t(X) Shoot\t(C) Dash\n\n";

	static PlatformerMotorComponent* pmc = m_Player->GetComponent<PlatformerMotorComponent>();
	if (pmc != nullptr)
	{
		m_Data += "Terrain: " + pmc->GetTerrainStateString() + "\n";
		m_Data += "Motion: " + pmc->GetMotionStateString() + "\n";
	}

	static TransformComponent* tc = m_Player->GetComponent<TransformComponent>();
	if (tc != nullptr)
	{
		const HGF::Vector2& position = tc->GetPosition();
		m_Data += "Position: { " + std::to_string(position.X) + ", " + std::to_string(position.Y) + "}\n";
		m_Camera.SetPosition(position);
	}

	static PhysicsComponent* pc = m_Player->GetComponent<PhysicsComponent>();
	if (pc != nullptr)
	{
		const HGF::Vector2& velocity = pc->GetVelocity();
		const HGF::Vector2& acceleration = pc->GetAcceleration();
		m_Data += "Velocity: { " + std::to_string(velocity.X) + ", " + std::to_string(velocity.Y) + "}\n";
		//m_Data += "Acceleration: { " + std::to_string(acceleration.X) + ", " + std::to_string(acceleration.Y) + "}";
	}

	return 0;
}

int GameplayScreen::Render(SAGE::SpriteBatch& pSpriteBatch)
{
	pSpriteBatch.Begin(m_SpriteEffect, m_Camera, SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended);
	m_World.Render(pSpriteBatch);
	pSpriteBatch.End();

	pSpriteBatch.Begin(m_SpriteEffect, SAGE::Camera2D::DefaultCamera, SAGE::SortMode::BackToFront, SAGE::BlendMode::AlphaBlended);
	pSpriteBatch.DrawString(m_SpriteFont, m_Data, HGF::Vector2(-620.0f, -340.0f), HGF::Color::White);
	pSpriteBatch.End();

	return 0;
}

int GameplayScreen::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	pGeometryBatch.Begin(m_GeometryEffect, m_Camera);
	m_World.Render(pGeometryBatch);
	pGeometryBatch.End();

	return 0;
}
