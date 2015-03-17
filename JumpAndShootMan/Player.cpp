#include "Player.hpp"
#include "Renderer.hpp"

const int Player::MaxRayCount = 12;

Player::Player()
{
}

Player::~Player()
{

}

bool Player::Load(const std::string& pFilename)
{
	if (!Texture.Load("data/img/megamanx.png"))
		return false;
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
	CollisionRays = new RayInfo[MaxRayCount];
	CollisionRays[MRK_HOTSPOT].Position = HGF::Vector2(Dimensions.X / 2.0f, Dimensions.Y);
	CollisionRays[MRK_HOTSPOT].Direction = Direction::Down;
	CollisionRays[MRK_HOTSPOT].Distance = 10.0f;
	CollisionRays[MRK_FOOT_LEFT].Position = HGF::Vector2(Dimensions.X * 1.0f / 8.0f, Dimensions.Y);
	CollisionRays[MRK_FOOT_LEFT].Direction = Direction::Down;
	CollisionRays[MRK_FOOT_LEFT].Distance = 10.0f;
	CollisionRays[MRK_FOOT_RIGHT].Position = HGF::Vector2(Dimensions.X * 7.0f / 8.0f, Dimensions.Y);
	CollisionRays[MRK_FOOT_RIGHT].Direction = Direction::Down;
	CollisionRays[MRK_FOOT_RIGHT].Distance = 10.0f;
	CollisionRays[MRK_LEFT_TOP].Position = HGF::Vector2(Dimensions.X * 1.0f / 16.0f, Dimensions.Y * 1.0f / 6.0f);
	CollisionRays[MRK_LEFT_TOP].Direction = Direction::Left;
	CollisionRays[MRK_LEFT_TOP].Distance = 10.0f;
	CollisionRays[MRK_LEFT_CENTER].Position = HGF::Vector2(Dimensions.X * 1.0f / 16.0f, Dimensions.Y * 3.0f / 6.0f);
	CollisionRays[MRK_LEFT_CENTER].Direction = Direction::Left;
	CollisionRays[MRK_LEFT_CENTER].Distance = 10.0f;
	CollisionRays[MRK_LEFT_BOTTOM].Position = HGF::Vector2(Dimensions.X * 1.0f / 16.0f, Dimensions.Y * 5.0f / 6.0f);
	CollisionRays[MRK_LEFT_BOTTOM].Direction = Direction::Left;
	CollisionRays[MRK_LEFT_BOTTOM].Distance = 10.0f;
	CollisionRays[MRK_RIGHT_TOP].Position = HGF::Vector2(Dimensions.X * 15.0f / 16.0f, Dimensions.Y * 1.0f / 6.0f);
	CollisionRays[MRK_RIGHT_TOP].Direction = Direction::Right;
	CollisionRays[MRK_RIGHT_TOP].Distance = 10.0f;
	CollisionRays[MRK_RIGHT_CENTER].Position = HGF::Vector2(Dimensions.X * 15.0f / 16.0f, Dimensions.Y * 3.0f / 6.0f);
	CollisionRays[MRK_RIGHT_CENTER].Direction = Direction::Right;
	CollisionRays[MRK_RIGHT_CENTER].Distance = 10.0f;
	CollisionRays[MRK_RIGHT_BOTTOM].Position = HGF::Vector2(Dimensions.X * 15.0f / 16.0f, Dimensions.Y * 5.0f / 6.0f);
	CollisionRays[MRK_RIGHT_BOTTOM].Direction = Direction::Right;
	CollisionRays[MRK_RIGHT_BOTTOM].Distance = 10.0f;
	CollisionRays[MRK_HEAD_CENTER].Position = HGF::Vector2(Dimensions.X * 3.0f / 6.0f, 0.0f);
	CollisionRays[MRK_HEAD_CENTER].Direction = Direction::Up;
	CollisionRays[MRK_HEAD_CENTER].Distance = 10.0f;
	CollisionRays[MRK_HEAD_LEFT].Position = HGF::Vector2(Dimensions.X * 2.0f / 6.0f, 0.0f);
	CollisionRays[MRK_HEAD_LEFT].Direction = Direction::Up;
	CollisionRays[MRK_HEAD_LEFT].Distance = 10.0f;
	CollisionRays[MRK_HEAD_RIGHT].Position = HGF::Vector2(Dimensions.X * 4.0f / 6.0f, 0.0f);
	CollisionRays[MRK_HEAD_RIGHT].Direction = Direction::Up;
	CollisionRays[MRK_HEAD_RIGHT].Distance = 10.0f;
}

bool Player::Render(const Renderer& pRenderer)
{
	// texture
	if (pRenderer.RenderTexture(Texture.GetID(), Position, Dimensions, HGF::Vector2::Zero, HGF::Vector2::One, IsFacingLeft) < 0)
		return false;

	// AABB
	if (pRenderer.RenderRectangle(Position, Dimensions, 0.0f, 1.0f, 0.0f) < 0)
		return false;

	// collision points
	for (int i = 0; i < Player::MaxRayCount; ++i)
	{
		if (pRenderer.RenderPoint(Position + CollisionRays[i].Position, 4.0f, 50.0f / 255.0f, 75.0f / 255.0f, 80.0f / 255.0f) < 0)
			return false;

		HGF::Vector2 offset;
		if (CollisionRays[i].Direction == Direction::Up)
			offset.Y = -CollisionRays[i].Distance;
		else if (CollisionRays[i].Direction == Direction::Down)
			offset.Y = CollisionRays[i].Distance;
		else if (CollisionRays[i].Direction == Direction::Left)
			offset.X = -CollisionRays[i].Distance;
		else if (CollisionRays[i].Direction == Direction::Right)
			offset.X = CollisionRays[i].Distance;

		if (pRenderer.RenderLine(Position + CollisionRays[i].Position, Position + CollisionRays[i].Position + offset, 1.0f, 0.7f, 0.0f, 0.0f, 1.0f) < 0)
			return false;
	}

	return true;
}