// GameplayScreen.hpp

#ifndef __GAMEPLAYSCREEN_HPP__
#define __GAMEPLAYSCREEN_HPP__

// SAGE Includes
#include <SAGE\Screen.hpp>
// Project Includes
#include "Player.hpp"
#include "World.hpp"

class GameplayScreen : public SAGE::Screen
{
	public:
		GameplayScreen(SAGE::ScreenManager* p_Manager);
		~GameplayScreen();

		int Initialize() override;
		int Finalize() override;

		int Update(float p_DeltaTime) override;
		int Render(SAGE::SpriteBatch& p_SpriteBatch) override;
		int Render(SAGE::GeometryBatch& p_GeometryBatch) override;

	private:
		SAGE::Camera2D m_Camera;
		HGF::Effect m_GeometryEffect;
		HGF::Effect m_SpriteEffect;
		SAGE::SpriteFont m_SpriteFont;
		World m_World;
		Player* m_Player;
		std::string m_Data;
};

#endif
