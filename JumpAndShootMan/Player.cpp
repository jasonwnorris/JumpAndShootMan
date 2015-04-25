// Player.cpp

// Project Includes
#include "Globals.hpp"
#include "Player.hpp"
#include "Renderer.hpp"
#include "DirectionalProjectile.hpp"

Player::Player(EntityManager* pManager) : Entity(pManager), ITiledTiledMapCollider(nullptr)
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

	// Up
	RaycastInfos[RayIndex::UpLeft].Position = HGF::Vector2(Dimensions.X * -1.0f / 4.0f, 0.0f);
	RaycastInfos[RayIndex::UpLeft].Direction = Direction::Up;
	RaycastInfos[RayIndex::UpLeft].Distance = updowndistance;
	RaycastInfos[RayIndex::UpLeft].Threshold = updowndistance;
	RaycastInfos[RayIndex::UpLeft].HasInterest = false;
	RaycastInfos[RayIndex::UpCenter].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[RayIndex::UpCenter].Direction = Direction::Up;
	RaycastInfos[RayIndex::UpCenter].Distance = updowndistance;
	RaycastInfos[RayIndex::UpCenter].Threshold = updowndistance + thresholdAddition;
	RaycastInfos[RayIndex::UpCenter].HasInterest = true;
	RaycastInfos[RayIndex::UpRight].Position = HGF::Vector2(Dimensions.X * 1.0f / 4.0f, 0.0f);
	RaycastInfos[RayIndex::UpRight].Direction = Direction::Up;
	RaycastInfos[RayIndex::UpRight].Distance = updowndistance;
	RaycastInfos[RayIndex::UpRight].Threshold = updowndistance;
	RaycastInfos[RayIndex::UpRight].HasInterest = false;
	// Down
	RaycastInfos[RayIndex::DownLeft].Position = HGF::Vector2(Dimensions.X * -3.0f / 10.0f, 0.0f);
	RaycastInfos[RayIndex::DownLeft].Direction = Direction::Down;
	RaycastInfos[RayIndex::DownLeft].Distance = updowndistance;
	RaycastInfos[RayIndex::DownLeft].Threshold = updowndistance;
	RaycastInfos[RayIndex::DownLeft].HasInterest = false;
	RaycastInfos[RayIndex::DownCenter].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[RayIndex::DownCenter].Direction = Direction::Down;
	RaycastInfos[RayIndex::DownCenter].Distance = updowndistance;
	RaycastInfos[RayIndex::DownCenter].Threshold = updowndistance + thresholdAddition;
	RaycastInfos[RayIndex::DownCenter].HasInterest = true;
	RaycastInfos[RayIndex::DownRight].Position = HGF::Vector2(Dimensions.X * 3.0f / 10.0f, 0.0f);
	RaycastInfos[RayIndex::DownRight].Direction = Direction::Down;
	RaycastInfos[RayIndex::DownRight].Distance = updowndistance;
	RaycastInfos[RayIndex::DownRight].Threshold = updowndistance;
	RaycastInfos[RayIndex::DownRight].HasInterest = false;
	// Left
	RaycastInfos[RayIndex::LeftTop].Position = HGF::Vector2(0.0f, Dimensions.Y * -1.0f / 3.0f);
	RaycastInfos[RayIndex::LeftTop].Direction = Direction::Left;
	RaycastInfos[RayIndex::LeftTop].Distance = leftrightdistance;
	RaycastInfos[RayIndex::LeftTop].Threshold = leftrightdistance;
	RaycastInfos[RayIndex::LeftTop].HasInterest = false;
	RaycastInfos[RayIndex::LeftCenter].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[RayIndex::LeftCenter].Direction = Direction::Left;
	RaycastInfos[RayIndex::LeftCenter].Distance = leftrightdistance;
	RaycastInfos[RayIndex::LeftCenter].Threshold = leftrightdistance + thresholdAddition;
	RaycastInfos[RayIndex::LeftCenter].HasInterest = false;
	RaycastInfos[RayIndex::LeftBottom].Position = HGF::Vector2(0.0f, Dimensions.Y * 1.0f / 3.0f);
	RaycastInfos[RayIndex::LeftBottom].Direction = Direction::Left;
	RaycastInfos[RayIndex::LeftBottom].Distance = leftrightdistance;
	RaycastInfos[RayIndex::LeftBottom].Threshold = leftrightdistance;
	RaycastInfos[RayIndex::LeftBottom].HasInterest = false;
	// Right
	RaycastInfos[RayIndex::RightTop].Position = HGF::Vector2(0.0f, Dimensions.Y * -1.0f / 3.0f);
	RaycastInfos[RayIndex::RightTop].Direction = Direction::Right;
	RaycastInfos[RayIndex::RightTop].Distance = leftrightdistance;
	RaycastInfos[RayIndex::RightTop].Threshold = leftrightdistance;
	RaycastInfos[RayIndex::RightTop].HasInterest = false;
	RaycastInfos[RayIndex::RightCenter].Position = HGF::Vector2(0.0f, 0.0f);
	RaycastInfos[RayIndex::RightCenter].Direction = Direction::Right;
	RaycastInfos[RayIndex::RightCenter].Distance = leftrightdistance;
	RaycastInfos[RayIndex::RightCenter].Threshold = leftrightdistance + thresholdAddition;
	RaycastInfos[RayIndex::RightCenter].HasInterest = false;
	RaycastInfos[RayIndex::RightBottom].Position = HGF::Vector2(0.0f, Dimensions.Y * 1.0f / 3.0f);
	RaycastInfos[RayIndex::RightBottom].Direction = Direction::Right;
	RaycastInfos[RayIndex::RightBottom].Distance = leftrightdistance;
	RaycastInfos[RayIndex::RightBottom].Threshold = leftrightdistance;
	RaycastInfos[RayIndex::RightBottom].HasInterest = false;

	return true;
}

void Player::Fire()
{
	std::shared_ptr<DirectionalProjectile> proj = mManager->Create<DirectionalProjectile>();
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
	for (int i = 0; i < MaxRayCount; ++i)
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
	for (int i = 0; i < MaxRayCount; ++i)
	{
		RaycastInfo& col = RaycastInfos[i];
		RaycastHit& hit = RaycastHits[i];

		pGeometryBatch.Draw(Position + col.Position, hit.Position, HGF::Color(0.9f, 0.1f, 0.8f));
	}
}