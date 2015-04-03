#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "Direction.hpp"
#include "Raycast.hpp"
#include "Texture.hpp"
#include "Vector2.hpp"

#include <vector>

class Renderer;
class Player
{
	public:
		Player();
		~Player();

		bool Load(const std::string& pFilename);
		bool Render(const Renderer& pRenderer);

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

		HGF::Texture Texture;
		HGF::Vector2 Position;
		HGF::Vector2 Velocity;
		HGF::Vector2 Acceleration;
		bool IsFacingLeft;
		float MovementSpeed;
		float JumpingSpeed;
		float Gravity;
		HGF::Vector2 Dimensions;
		bool IsGrounded;
		bool IsJumping;
		RayInfo* CollisionRays;
		RaycastHit* RaycastHits;
};

#endif