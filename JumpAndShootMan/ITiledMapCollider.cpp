// ITiledMapCollider.cpp

// Project Includes
#include "ITiledMapCollider.hpp"

ITiledMapCollider::ITiledMapCollider(World* pWorld)
{
	mWorld = pWorld;

	// Up
	RaycastInfos[RayIndex::UpLeft].Direction = Direction::Up;
	RaycastInfos[RayIndex::UpLeft].HasInterest = false;
	RaycastInfos[RayIndex::UpCenter].Direction = Direction::Up;
	RaycastInfos[RayIndex::UpCenter].HasInterest = true;
	RaycastInfos[RayIndex::UpRight].Direction = Direction::Up;
	RaycastInfos[RayIndex::UpRight].HasInterest = false;
	// Down
	RaycastInfos[RayIndex::DownLeft].Direction = Direction::Down;
	RaycastInfos[RayIndex::DownLeft].HasInterest = false;
	RaycastInfos[RayIndex::DownCenter].Direction = Direction::Down;
	RaycastInfos[RayIndex::DownCenter].HasInterest = true;
	RaycastInfos[RayIndex::DownRight].Direction = Direction::Down;
	RaycastInfos[RayIndex::DownRight].HasInterest = false;
	// Left
	RaycastInfos[RayIndex::LeftTop].Direction = Direction::Left;
	RaycastInfos[RayIndex::LeftTop].HasInterest = false;
	RaycastInfos[RayIndex::LeftCenter].Direction = Direction::Left;
	RaycastInfos[RayIndex::LeftCenter].HasInterest = false;
	RaycastInfos[RayIndex::LeftBottom].Direction = Direction::Left;
	RaycastInfos[RayIndex::LeftBottom].HasInterest = false;
	// Right
	RaycastInfos[RayIndex::RightTop].Direction = Direction::Right;
	RaycastInfos[RayIndex::RightTop].HasInterest = false;
	RaycastInfos[RayIndex::RightCenter].Direction = Direction::Right;
	RaycastInfos[RayIndex::RightCenter].HasInterest = false;
	RaycastInfos[RayIndex::RightBottom].Direction = Direction::Right;
	RaycastInfos[RayIndex::RightBottom].HasInterest = false;
}

ITiledMapCollider::~ITiledMapCollider()
{
	mWorld = nullptr;
}

void ITiledMapCollider::SetTriRay(Direction pDirection, HGF::Vector2& pPosition, float pSpread, float pMinimum, float pMaximum)
{
	switch (pDirection)
	{
	case Direction::Up:
		RaycastInfos[RayIndex::UpLeft].Position = pPosition + HGF::Vector2(-pSpread / 2.0f, 0.0f);
		RaycastInfos[RayIndex::UpLeft].Distance = pMinimum;
		RaycastInfos[RayIndex::UpLeft].Threshold = pMinimum;
		RaycastInfos[RayIndex::UpCenter].Position = pPosition + HGF::Vector2(0.0f, 0.0f);
		RaycastInfos[RayIndex::UpCenter].Distance = pMinimum;
		RaycastInfos[RayIndex::UpCenter].Threshold = pMaximum;
		RaycastInfos[RayIndex::UpRight].Position = pPosition + HGF::Vector2(pSpread / 2.0f, 0.0f);
		RaycastInfos[RayIndex::UpRight].Distance = pMinimum;
		RaycastInfos[RayIndex::UpRight].Threshold = pMinimum;
		break;
	case Direction::Down:
		RaycastInfos[RayIndex::DownLeft].Position = pPosition + HGF::Vector2(-pSpread / 2.0f, 0.0f);
		RaycastInfos[RayIndex::DownLeft].Distance = pMinimum;
		RaycastInfos[RayIndex::DownLeft].Threshold = pMinimum;
		RaycastInfos[RayIndex::DownCenter].Position = pPosition + HGF::Vector2(0.0f, 0.0f);
		RaycastInfos[RayIndex::DownCenter].Distance = pMinimum;
		RaycastInfos[RayIndex::DownCenter].Threshold = pMaximum;
		RaycastInfos[RayIndex::DownRight].Position = pPosition + HGF::Vector2(pSpread / 2.0f, 0.0f);
		RaycastInfos[RayIndex::DownRight].Distance = pMinimum;
		RaycastInfos[RayIndex::DownRight].Threshold = pMinimum;
		break;
	case Direction::Left:
		RaycastInfos[RayIndex::LeftTop].Position = pPosition + HGF::Vector2(0.0f, -pSpread / 2.0f);
		RaycastInfos[RayIndex::LeftTop].Distance = pMinimum;
		RaycastInfos[RayIndex::LeftTop].Threshold = pMinimum;
		RaycastInfos[RayIndex::LeftCenter].Position = pPosition + HGF::Vector2(0.0f, 0.0f);
		RaycastInfos[RayIndex::LeftCenter].Distance = pMinimum;
		RaycastInfos[RayIndex::LeftCenter].Threshold = pMaximum;
		RaycastInfos[RayIndex::LeftBottom].Position = pPosition + HGF::Vector2(0.0f, pSpread / 2.0f);
		RaycastInfos[RayIndex::LeftBottom].Distance = pMinimum;
		RaycastInfos[RayIndex::LeftBottom].Threshold = pMinimum;
		break;
	case Direction::Right:
		RaycastInfos[RayIndex::RightTop].Position = pPosition + HGF::Vector2(0.0f, -pSpread / 2.0f);
		RaycastInfos[RayIndex::RightTop].Distance = pMinimum;
		RaycastInfos[RayIndex::RightTop].Threshold = pMinimum;
		RaycastInfos[RayIndex::RightCenter].Position = pPosition + HGF::Vector2(0.0f, 0.0f);
		RaycastInfos[RayIndex::RightCenter].Distance = pMinimum;
		RaycastInfos[RayIndex::RightCenter].Threshold = pMaximum;
		RaycastInfos[RayIndex::RightBottom].Position = pPosition + HGF::Vector2(0.0f, pSpread / 2.0f);
		RaycastInfos[RayIndex::RightBottom].Distance = pMinimum;
		RaycastInfos[RayIndex::RightBottom].Threshold = pMinimum;
		break;
	}
}

void ITiledMapCollider::CheckCollision()
{
	const TiledMap* map = mWorld->GetMap();

	for (int i = 0; i < MaxRayCount; i += 3)
	{
		map->Raycast(RaycastInfos[i].Position, RaycastInfos[i].Direction, RaycastInfos[i].HasInterest, RaycastHits[i]);

		if (RaycastHits[i].Distance < RaycastInfos[i].Distance)
		{
			// fire event for middle hit
			OnCollision(RaycastInfos[i].Direction, RaycastInfos[i].Distance - RaycastHits[i].Distance);
		}
		else if (RaycastHits[i].Distance < RaycastInfos[i].Threshold)
		{
			// fire event for middle threshold
			OnCollision(RaycastInfos[i].Direction, RaycastInfos[i].Distance - RaycastHits[i].Distance);
		}
		else
		{
			bool hit[2];
			for (int j = 1; j < 3; ++j)
			{
				map->Raycast(RaycastInfos[i + j].Position, RaycastInfos[i].Direction, RaycastInfos[i + j].HasInterest, RaycastHits[i + j]);
				hit[j - 1] = RaycastHits[i + j].Distance < RaycastInfos[i + j].Threshold;
			}

			if (hit[0] && hit[1])
			{
				if (RaycastHits[i + 1].Distance < RaycastHits[i + 2].Distance)
				{
					// fire event for left/top distance
					OnCollision(RaycastInfos[i + 1].Direction, RaycastInfos[i + 1].Distance - RaycastHits[i + 1].Distance);
				}
				else
				{
					// fire event for right/bottom distance
					OnCollision(RaycastInfos[i + 2].Direction, RaycastInfos[i + 2].Distance - RaycastHits[i + 2].Distance);
				}
			}
			else if (hit[0])
			{
				// fire event for left/top distance
				OnCollision(RaycastInfos[i + 1].Direction, RaycastInfos[i + 1].Distance - RaycastHits[i + 1].Distance);
			}
			else if (hit[1])
			{
				// fire event for right/bottom distance
				OnCollision(RaycastInfos[i + 2].Direction, RaycastInfos[i + 2].Distance - RaycastHits[i + 2].Distance);
			}
			else
			{
				// fire event for nothing
				OnEmpty(RaycastInfos[i].Direction, std::fminf(RaycastHits[i].Distance, std::fminf(RaycastHits[i + 1].Distance, RaycastHits[i + 2].Distance)));
			}
		}
	}
}

void ITiledMapCollider::Render(SAGE::GeometryBatch& pGeometryBatch)
{
	for (int i = 0; i < MaxRayCount; ++i)
	{
		RaycastInfo& col = RaycastInfos[i];
		RaycastHit& hit = RaycastHits[i];

		pGeometryBatch.Draw(col.Position, hit.Position, HGF::Color(0.9f, 0.1f, 0.8f));
	}
}