// Player.cpp

// Project Includes
#include "Globals.hpp"
#include "Player.hpp"
#include "Renderer.hpp"
#include "DirectionalProjectile.hpp"

const int Player::MaxRayCount = 12;

Player::Player(EntityManager* pManager) : Entity(pManager)
{
	Position = HGF::Vector2(100.0f, 100.0f);
	Velocity = HGF::Vector2::Zero;
	Acceleration = HGF::Vector2::Zero;
	IsFacingLeft = false;
	MovementSpeed = 0.195f;
	JumpingSpeed = 1.75f;
	Gravity = 0.055f;
	Source = HGF::Rectangle(11, 107, 108, 149);
	Scale = HGF::Vector2(0.5f, 0.5f);
	Origin = HGF::Vector2(Source.Width / 2.0f, Source.Height / 2.0f);
	Dimensions = HGF::Vector2(Source.Width * Scale.X, Source.Height * Scale.Y);
	IsGrounded = false;
	IsJumping = false;
	IsDebugFly = false;
}

Player::~Player()
{
	delete [] RaycastInfos;
	delete [] RaycastHits;
}

bool Player::Load(const std::string& pFilename)
{
	if (!mTexture.Load(pFilename.c_str()))
		return false;

	if (!DirectionalProjectile::DirProjTexture.Load("data/img/projectile.png"))
		return false;

	float updowndistance = 70.0f / 2.0f;
	float leftrightdistance = 46.0f / 2.0f;
	float thresholdAddition = 25.0f;

	RaycastInfos = new RaycastInfo[MaxRayCount];
	// Up
	RaycastInfos[MRK_HEAD_LEFT].Position = HGF::Vector2(Dimensions.X * -1.0f / 4.0f, 0.0f);
	RaycastInfos[MRK_HEAD_LEFT].Direction = Direction::Up;
	RaycastInfos[MRK_HEAD_LEFT].Distance = updowndistance;
	RaycastInfos[MRK_HEAD_LEFT].Threshold = updowndistance;
	RaycastInfos[MRK_HEAD_LEFT].HasInterest = false;
	RaycastInfos[MRK_HEAD_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[MRK_HEAD_CENTER].Direction = Direction::Up;
	RaycastInfos[MRK_HEAD_CENTER].Distance = updowndistance;
	RaycastInfos[MRK_HEAD_CENTER].Threshold = updowndistance + thresholdAddition;
	RaycastInfos[MRK_HEAD_CENTER].HasInterest = true;
	RaycastInfos[MRK_HEAD_RIGHT].Position = HGF::Vector2(Dimensions.X * 1.0f / 4.0f, 0.0f);
	RaycastInfos[MRK_HEAD_RIGHT].Direction = Direction::Up;
	RaycastInfos[MRK_HEAD_RIGHT].Distance = updowndistance;
	RaycastInfos[MRK_HEAD_RIGHT].Threshold = updowndistance;
	RaycastInfos[MRK_HEAD_RIGHT].HasInterest = false;
	// Down
	RaycastInfos[MRK_FOOT_LEFT].Position = HGF::Vector2(Dimensions.X * -3.0f / 10.0f, 0.0f);
	RaycastInfos[MRK_FOOT_LEFT].Direction = Direction::Down;
	RaycastInfos[MRK_FOOT_LEFT].Distance = updowndistance;
	RaycastInfos[MRK_FOOT_LEFT].Threshold = updowndistance;
	RaycastInfos[MRK_FOOT_LEFT].HasInterest = false;
	RaycastInfos[MRK_HOTSPOT].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[MRK_HOTSPOT].Direction = Direction::Down;
	RaycastInfos[MRK_HOTSPOT].Distance = updowndistance;
	RaycastInfos[MRK_HOTSPOT].Threshold = updowndistance + thresholdAddition;
	RaycastInfos[MRK_HOTSPOT].HasInterest = true;
	RaycastInfos[MRK_FOOT_RIGHT].Position = HGF::Vector2(Dimensions.X * 3.0f / 10.0f, 0.0f);
	RaycastInfos[MRK_FOOT_RIGHT].Direction = Direction::Down;
	RaycastInfos[MRK_FOOT_RIGHT].Distance = updowndistance;
	RaycastInfos[MRK_FOOT_RIGHT].Threshold = updowndistance;
	RaycastInfos[MRK_FOOT_RIGHT].HasInterest = false;
	// Left
	RaycastInfos[MRK_LEFT_TOP].Position = HGF::Vector2(0.0f, Dimensions.Y * -1.0f / 3.0f);
	RaycastInfos[MRK_LEFT_TOP].Direction = Direction::Left;
	RaycastInfos[MRK_LEFT_TOP].Distance = leftrightdistance;
	RaycastInfos[MRK_LEFT_TOP].Threshold = leftrightdistance;
	RaycastInfos[MRK_LEFT_TOP].HasInterest = false;
	RaycastInfos[MRK_LEFT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[MRK_LEFT_CENTER].Direction = Direction::Left;
	RaycastInfos[MRK_LEFT_CENTER].Distance = leftrightdistance;
	RaycastInfos[MRK_LEFT_CENTER].Threshold = leftrightdistance + thresholdAddition;
	RaycastInfos[MRK_LEFT_CENTER].HasInterest = false;
	RaycastInfos[MRK_LEFT_BOTTOM].Position = HGF::Vector2(0.0f, Dimensions.Y * 1.0f / 3.0f);
	RaycastInfos[MRK_LEFT_BOTTOM].Direction = Direction::Left;
	RaycastInfos[MRK_LEFT_BOTTOM].Distance = leftrightdistance;
	RaycastInfos[MRK_LEFT_BOTTOM].Threshold = leftrightdistance;
	RaycastInfos[MRK_LEFT_BOTTOM].HasInterest = false;
	// Right
	RaycastInfos[MRK_RIGHT_TOP].Position = HGF::Vector2(0.0f, Dimensions.Y * -1.0f / 3.0f);
	RaycastInfos[MRK_RIGHT_TOP].Direction = Direction::Right;
	RaycastInfos[MRK_RIGHT_TOP].Distance = leftrightdistance;
	RaycastInfos[MRK_RIGHT_TOP].Threshold = leftrightdistance;
	RaycastInfos[MRK_RIGHT_TOP].HasInterest = false;
	RaycastInfos[MRK_RIGHT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[MRK_RIGHT_CENTER].Direction = Direction::Right;
	RaycastInfos[MRK_RIGHT_CENTER].Distance = leftrightdistance;
	RaycastInfos[MRK_RIGHT_CENTER].Threshold = leftrightdistance + thresholdAddition;
	RaycastInfos[MRK_RIGHT_CENTER].HasInterest = false;
	RaycastInfos[MRK_RIGHT_BOTTOM].Position = HGF::Vector2(0.0f, Dimensions.Y * 1.0f / 3.0f);
	RaycastInfos[MRK_RIGHT_BOTTOM].Direction = Direction::Right;
	RaycastInfos[MRK_RIGHT_BOTTOM].Distance = leftrightdistance;
	RaycastInfos[MRK_RIGHT_BOTTOM].Threshold = leftrightdistance;
	RaycastInfos[MRK_RIGHT_BOTTOM].HasInterest = false;

	RaycastHits = new RaycastHit[MaxRayCount];

	return true;
}

void Player::Fire()
{
	DirectionalProjectile* proj = mManager->Create<DirectionalProjectile>();
	proj->Position = Position;
	proj->Speed = 250.0f;
	proj->LifeTime = 2.5f;
	proj->Direction = IsFacingLeft ? Direction::Left : Direction::Right;
}

void Player::Update(float pDeltaTime)
{

}

void Player::Render(const Renderer& pRenderer)
{
	pRenderer.RenderTexture(mTexture, Position, Dimensions, Dimensions / 2.0f, HGF::Vector2(12, 110), HGF::Vector2(118, 256), IsFacingLeft);
}

void Player::RenderDebug(const Renderer& pRenderer)
{
	// AABB
	pRenderer.RenderRectangle(Position, Dimensions, Dimensions / 2.0f, 0.0f, 1.0f, 0.0f);

	// collision points
	for (int i = 0; i < Player::MaxRayCount; ++i)
	{
		RaycastInfo& col = RaycastInfos[i];
		RaycastHit& hit = RaycastHits[i];

		pRenderer.RenderLine(Position + col.Position, hit.Position, 1.0f, 0.9f, 0.1f, 0.8f, 1.0f);
		pRenderer.RenderPoint(hit.Position, 4.0f, 1.0f, 1.0f, 0.0f);
	}
}

void Player::Render(SAGE::SpriteBatch& pSpriteBatch)
{	
	pSpriteBatch.Draw(mTexture,
					  Position,
					  Source,
					  HGF::Color::White,
					  Origin,
					  0.0f,
					  Scale,
					  IsFacingLeft ? SAGE::Orientation::FlipHorizontal : SAGE::Orientation::None);
}

void Player::RenderDebug(SAGE::GeometryBatch& pGeometryBatch)
{
	// AABB
	HGF::Vector2 TL(Position.X - Dimensions.X / 2.0f, Position.Y - Dimensions.Y / 2.0f);
	HGF::Vector2 BR(Position.X + Dimensions.X / 2.0f, Position.Y + Dimensions.Y / 2.0f);
	pGeometryBatch.Draw(HGF::Vector2(TL.X, TL.Y), HGF::Vector2(BR.X, TL.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(BR.X, TL.Y), HGF::Vector2(BR.X, BR.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(BR.X, BR.Y), HGF::Vector2(TL.X, BR.Y), HGF::Color::Green);
	pGeometryBatch.Draw(HGF::Vector2(TL.X, BR.Y), HGF::Vector2(TL.X, TL.Y), HGF::Color::Green);

	// collision points
	for (int i = 0; i < Player::MaxRayCount; ++i)
	{
		RaycastInfo& col = RaycastInfos[i];
		RaycastHit& hit = RaycastHits[i];

		pGeometryBatch.Draw(Position + col.Position, hit.Position, HGF::Color(0.9f, 0.1f, 0.8f));
	}
}