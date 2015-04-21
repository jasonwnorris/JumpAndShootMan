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
#include "EntityManager.hpp"
#include "Raycast.hpp"
// STL Includes
#include <vector>

class Renderer;
class Player : public Entity
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

		static const int MaxRayCount;
		enum RayIndex
		{
			MRK_HOTSPOT,
			MRK_FOOT_LEFT,
			MRK_FOOT_RIGHT,
			MRK_HEAD_CENTER,
			MRK_HEAD_LEFT,
			MRK_HEAD_RIGHT,
			MRK_LEFT_CENTER,
			MRK_LEFT_TOP,
			MRK_LEFT_BOTTOM,
			MRK_RIGHT_CENTER,
			MRK_RIGHT_TOP,
			MRK_RIGHT_BOTTOM,
		};

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
		RaycastInfo* RaycastInfos;
		RaycastHit* RaycastHits;

	private:
		HGF::Texture mTexture;
};

#endif