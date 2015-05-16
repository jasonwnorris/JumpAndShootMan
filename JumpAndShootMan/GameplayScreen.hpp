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
		GameplayScreen(SAGE::ScreenManager* pManager);
		~GameplayScreen();

		int Initialize() override;
		int Finalize() override;

		int Update(float pDeltaTime) override;
		int Render(SAGE::SpriteBatch& pSpriteBatch) override;
		int Render(SAGE::GeometryBatch& pGeometryBatch) override;

	private:
		SAGE::Camera2D mCamera;
		HGF::Effect mGeometryEffect;
		HGF::Effect mSpriteEffect;
		SAGE::SpriteFont mSpriteFont;
		World mWorld;
		Player* mPlayer;
		std::string mData;
};

#endif