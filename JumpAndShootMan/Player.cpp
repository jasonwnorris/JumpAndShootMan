#include "Globals.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

const int Player::MaxRayCount = 12;

Player::Player()
{
	Position = HGF::Vector2(100.0f, 100.0f);
	Velocity = HGF::Vector2::Zero;
	Acceleration = HGF::Vector2::Zero;
	IsFacingLeft = false;
	MovementSpeed = 0.185f + 1.5f;
	JumpingSpeed = 1.15f;
	Gravity = 0.035f;
	Dimensions = HGF::Vector2(107.0f / 5.0f, 147.0f / 5.0f);
	IsGrounded = false;
	IsJumping = false;
}

Player::~Player()
{
	delete [] CollisionRays;
}

bool Player::Load(const std::string& pFilename)
{
	if (!Texture.Load("data/img/player.png"))
		return false;

	CollisionRays = new RayInfo[MaxRayCount];
	// Down
	CollisionRays[MRK_FOOT_LEFT].Position = HGF::Vector2(Dimensions.X * -3.0f / 8.0f, 0.0f);
	CollisionRays[MRK_FOOT_LEFT].Direction = Direction::Down;
	CollisionRays[MRK_FOOT_LEFT].MaxDistance = 14.75f;
	CollisionRays[MRK_FOOT_LEFT].Targets.push_back(0);
	CollisionRays[MRK_HOTSPOT].Position = HGF::Vector2(0.0f, 0.0f);
	CollisionRays[MRK_HOTSPOT].Direction = Direction::Down;
	CollisionRays[MRK_HOTSPOT].MaxDistance = 14.75f;
	CollisionRays[MRK_HOTSPOT].Targets.push_back(0);
	CollisionRays[MRK_HOTSPOT].Targets.push_back(1);
	CollisionRays[MRK_HOTSPOT].Targets.push_back(2);
	CollisionRays[MRK_FOOT_RIGHT].Position = HGF::Vector2(Dimensions.X * 3.0f / 8.0f, 0.0f);
	CollisionRays[MRK_FOOT_RIGHT].Direction = Direction::Down;
	CollisionRays[MRK_FOOT_RIGHT].MaxDistance = 14.75f;
	CollisionRays[MRK_FOOT_RIGHT].Targets.push_back(0);
	// Left
	CollisionRays[MRK_LEFT_TOP].Position = HGF::Vector2(0.0f, Dimensions.Y * -1.0f / 3.0f);
	CollisionRays[MRK_LEFT_TOP].Direction = Direction::Left;
	CollisionRays[MRK_LEFT_TOP].MaxDistance = 11.0f;
	CollisionRays[MRK_LEFT_TOP].Targets.push_back(0);
	CollisionRays[MRK_LEFT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	CollisionRays[MRK_LEFT_CENTER].Direction = Direction::Left;
	CollisionRays[MRK_LEFT_CENTER].MaxDistance = 11.0f;
	CollisionRays[MRK_LEFT_CENTER].Targets.push_back(0);
	CollisionRays[MRK_LEFT_BOTTOM].Position = HGF::Vector2(0.0f, Dimensions.Y * 1.0f / 3.0f);
	CollisionRays[MRK_LEFT_BOTTOM].Direction = Direction::Left;
	CollisionRays[MRK_LEFT_BOTTOM].MaxDistance = 11.0f;
	CollisionRays[MRK_LEFT_BOTTOM].Targets.push_back(0);
	// Right
	CollisionRays[MRK_RIGHT_TOP].Position = HGF::Vector2(0.0f, Dimensions.Y * -1.0f / 3.0f);
	CollisionRays[MRK_RIGHT_TOP].Direction = Direction::Right;
	CollisionRays[MRK_RIGHT_TOP].MaxDistance = 11.0f;
	CollisionRays[MRK_RIGHT_TOP].Targets.push_back(0);
	CollisionRays[MRK_RIGHT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	CollisionRays[MRK_RIGHT_CENTER].Direction = Direction::Right;
	CollisionRays[MRK_RIGHT_CENTER].MaxDistance = 11.0f;
	CollisionRays[MRK_RIGHT_CENTER].Targets.push_back(0);
	CollisionRays[MRK_RIGHT_BOTTOM].Position = HGF::Vector2(0.0f, Dimensions.Y * 1.0f / 3.0f);
	CollisionRays[MRK_RIGHT_BOTTOM].Direction = Direction::Right;
	CollisionRays[MRK_RIGHT_BOTTOM].MaxDistance = 11.0f;
	CollisionRays[MRK_RIGHT_BOTTOM].Targets.push_back(0);
	// Up
	CollisionRays[MRK_HEAD_LEFT].Position = HGF::Vector2(Dimensions.X * -1.0f / 4.0f, 0.0f);
	CollisionRays[MRK_HEAD_LEFT].Direction = Direction::Up;
	CollisionRays[MRK_HEAD_LEFT].MaxDistance = 14.75f;
	CollisionRays[MRK_HEAD_LEFT].Targets.push_back(0);
	CollisionRays[MRK_HEAD_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	CollisionRays[MRK_HEAD_CENTER].Direction = Direction::Up;
	CollisionRays[MRK_HEAD_CENTER].MaxDistance = 14.75f;
	CollisionRays[MRK_HEAD_CENTER].Targets.push_back(0);
	CollisionRays[MRK_HEAD_RIGHT].Position = HGF::Vector2(Dimensions.X * 1.0f / 4.0f, 0.0f);
	CollisionRays[MRK_HEAD_RIGHT].Direction = Direction::Up;
	CollisionRays[MRK_HEAD_RIGHT].MaxDistance = 14.75f;
	CollisionRays[MRK_HEAD_RIGHT].Targets.push_back(0);

	return false;
}

bool Player::Render(const Renderer& pRenderer)
{
	// texture
	if (pRenderer.RenderTexture(Texture, Position, Dimensions, Dimensions / 2.0f, HGF::Vector2(12, 110), HGF::Vector2(118, 256), IsFacingLeft) < 0)
		return false;
	
	if (Globals::IsDebugDrawOn)
	{
		// AABB
		if (pRenderer.RenderRectangle(Position, Dimensions, Dimensions / 2.0f, 0.0f, 1.0f, 0.0f) < 0)
			return false;

		// collision points
		for (int i = 0; i < Player::MaxRayCount; ++i)
		{
			RayInfo& col = CollisionRays[i];

			if (pRenderer.RenderLine(Position + col.Position, col.OutHit, 1.0f, 0.9f, 0.1f, 0.8f, 1.0f) < 0)
				return false;

			if (pRenderer.RenderPoint(col.OutHit, 4.0f, 1.0f, 1.0f, 0.0f) < 0)
				return false;
		}
	}
	
	return true;
}