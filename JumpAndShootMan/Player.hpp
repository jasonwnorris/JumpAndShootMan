// Player.hpp

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// HGF Includes
#include <HGF\Texture.hpp>
#include <HGF\Vector2.hpp>
// Project Includes
#include "ITiledMapCollider.hpp"
#include "EntityManager.hpp"
#include "Raycast.hpp"
// STL Includes
#include <vector>

class Renderer;
class Player : public Entity, public ITiledTiledMapCollider
{
	public:
		Player(EntityManager* pManager);
		~Player();

		bool Load(const std::string& pFilename);
		void Fire();

		void Update(float pDeltaTime) override;
		void Render(const Renderer& pRenderer) override;
		void RenderDebug(const Renderer& pRenderer);
		void Render(SAGE::SpriteBatch& pSpriteBatch) override;
		void RenderDebug(SAGE::GeometryBatch& pGeometryBatch);

		HGF::Vector2 Position;
		HGF::Vector2 Velocity;
		HGF::Vector2 Acceleration;
		bool IsFacingLeft;
		float MovementSpeed;
		float JumpingSpeed;
		float Gravity;
		HGF::Rectangle Source;
		HGF::Vector2 Scale;
		HGF::Vector2 Origin;
		HGF::Vector2 Dimensions;
		bool IsGrounded;
		bool IsJumping;
		bool IsDebugFly;

	private:
		HGF::Texture mTexture;
};

#endif