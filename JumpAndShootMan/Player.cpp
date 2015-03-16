#include "Player.hpp"

const int Player::MaxMarkerCount = 12;

Player::Player()
{
	Position = HGF::Vector2(100.0f, 100.0f);
	Velocity = HGF::Vector2::Zero;
	Acceleration = HGF::Vector2::Zero;
	IsFacingLeft = false;
	MovementSpeed = 0.185f;
	JumpingSpeed = 1.15f;
	Gravity = 0.035f;
	Dimensions = HGF::Vector2(30, 34);
	IsGrounded = false;
	IsJumping = false;
	CollisionMarkers = new CollisionMarker[MaxMarkerCount];
	for (int i = 0; i < MaxMarkerCount; ++i)
		CollisionMarkers[i].IsTouching = false;
	CollisionMarkers[MRK_HOTSPOT].Position = HGF::Vector2(Dimensions.X / 2.0f, Dimensions.Y);
	CollisionMarkers[MRK_FOOT_LEFT].Position = HGF::Vector2(Dimensions.X * 1.0f / 8.0f, Dimensions.Y);
	CollisionMarkers[MRK_FOOT_RIGHT].Position = HGF::Vector2(Dimensions.X * 7.0f / 8.0f, Dimensions.Y);
	CollisionMarkers[MRK_LEFT_TOP].Position = HGF::Vector2(Dimensions.X * 1.0f / 16.0f, Dimensions.Y * 1.0f / 6.0f);
	CollisionMarkers[MRK_LEFT_CENTER].Position = HGF::Vector2(Dimensions.X * 1.0f / 16.0f, Dimensions.Y * 3.0f / 6.0f);
	CollisionMarkers[MRK_LEFT_BOTTOM].Position = HGF::Vector2(Dimensions.X * 1.0f / 16.0f, Dimensions.Y * 5.0f / 6.0f);
	CollisionMarkers[MRK_RIGHT_TOP].Position = HGF::Vector2(Dimensions.X * 15.0f / 16.0f, Dimensions.Y * 1.0f / 6.0f);
	CollisionMarkers[MRK_RIGHT_CENTER].Position = HGF::Vector2(Dimensions.X * 15.0f / 16.0f, Dimensions.Y * 3.0f / 6.0f);
	CollisionMarkers[MRK_RIGHT_BOTTOM].Position = HGF::Vector2(Dimensions.X * 15.0f / 16.0f, Dimensions.Y * 5.0f / 6.0f);
	CollisionMarkers[MRK_HEAD_CENTER].Position = HGF::Vector2(Dimensions.X * 3.0f / 6.0f, 0.0f);
	CollisionMarkers[MRK_HEAD_LEFT].Position = HGF::Vector2(Dimensions.X * 2.0f / 6.0f, 0.0f);
	CollisionMarkers[MRK_HEAD_RIGHT].Position = HGF::Vector2(Dimensions.X * 4.0f / 6.0f, 0.0f);
}

Player::~Player()
{

}