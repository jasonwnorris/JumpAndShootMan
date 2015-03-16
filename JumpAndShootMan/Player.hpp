#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include "Vector2.hpp"

struct CollisionMarker
{
	HGF::Vector2 Position;
	bool IsTouching;
};

class Player
{
	public:
		Player();
		~Player();

		static const int MaxMarkerCount;
		enum MarkerIndex
		{
			MRK_HOTSPOT,
			MRK_FOOT_LEFT,
			MRK_FOOT_RIGHT,
			MRK_LEFT_CENTER,
			MRK_LEFT_TOP,
			MRK_LEFT_BOTTOM,
			MRK_RIGHT_CENTER,
			MRK_RIGHT_TOP,
			MRK_RIGHT_BOTTOM,
			MRK_HEAD_CENTER,
			MRK_HEAD_LEFT,
			MRK_HEAD_RIGHT
		};

		unsigned int TextureID;
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
		CollisionMarker* CollisionMarkers;
};

#endif