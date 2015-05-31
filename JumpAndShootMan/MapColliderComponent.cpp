// MapColliderComponent.cpp

// HGF Includes
#include <HGF\MathUtil.hpp>
// Project Includes
#include "MapColliderComponent.hpp"
#include "Entity.hpp"
#include "World.hpp"

MapColliderComponent::MapColliderComponent(Entity* p_Owner) : IRenderComponent(p_Owner)
{
	m_Name = "MapColliderComponent";

	m_World_TiledMap = m_Owner->GetWorld()->GetMap();
	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();
	
	m_UpHitDistance = 35.0f;
	m_DownHitDistance = 40.0f;
	m_LeftHitDistance = 25.0f;
	m_RightHitDistance = 25.0f;

	float upSpread = 14.0f;
	float downSpreed = 18.0f;
	float sideSpread = 22.5f;

	m_Rays[UP_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	m_Rays[UP_CENTER].Direction = HGF::Vector2::Up;
	m_Rays[UP_CENTER].Mask = 2;
	m_Rays[UP_LEFT].Position = HGF::Vector2(-upSpread, 0.0f);
	m_Rays[UP_LEFT].Direction = HGF::Vector2::Up;
	m_Rays[UP_LEFT].Mask = 1;
	m_Rays[UP_RIGHT].Position = HGF::Vector2(upSpread, 0.0f);
	m_Rays[UP_RIGHT].Direction = HGF::Vector2::Up;
	m_Rays[UP_RIGHT].Mask = 1;

	m_Rays[DOWN_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	m_Rays[DOWN_CENTER].Direction = HGF::Vector2::Down;
	m_Rays[DOWN_CENTER].Mask = 2;
	m_Rays[DOWN_LEFT].Position = HGF::Vector2(-downSpreed, 0.0f);
	m_Rays[DOWN_LEFT].Direction = HGF::Vector2::Down;
	m_Rays[DOWN_LEFT].Mask = 1;
	m_Rays[DOWN_RIGHT].Position = HGF::Vector2(downSpreed, 0.0f);
	m_Rays[DOWN_RIGHT].Direction = HGF::Vector2::Down;
	m_Rays[DOWN_RIGHT].Mask = 1;

	m_Rays[LEFT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	m_Rays[LEFT_CENTER].Direction = HGF::Vector2::Left;
	m_Rays[LEFT_CENTER].Mask = 2;
	m_Rays[LEFT_TOP].Position = HGF::Vector2(0.0f, -sideSpread);
	m_Rays[LEFT_TOP].Direction = HGF::Vector2::Left;
	m_Rays[LEFT_TOP].Mask = 1;
	m_Rays[LEFT_BOTTOM].Position = HGF::Vector2(0.0f, sideSpread);
	m_Rays[LEFT_BOTTOM].Direction = HGF::Vector2::Left;
	m_Rays[LEFT_BOTTOM].Mask = 1;

	m_Rays[RIGHT_CENTER].Position = HGF::Vector2(0.0f, 0.0f);
	m_Rays[RIGHT_CENTER].Direction = HGF::Vector2::Right;
	m_Rays[RIGHT_CENTER].Mask = 2;
	m_Rays[RIGHT_TOP].Position = HGF::Vector2(0.0f, -sideSpread);
	m_Rays[RIGHT_TOP].Direction = HGF::Vector2::Right;
	m_Rays[RIGHT_TOP].Mask = 1;
	m_Rays[RIGHT_BOTTOM].Position = HGF::Vector2(0.0f, sideSpread);
	m_Rays[RIGHT_BOTTOM].Direction = HGF::Vector2::Right;
	m_Rays[RIGHT_BOTTOM].Mask = 1;
}

MapColliderComponent::~MapColliderComponent()
{
}

float MapColliderComponent::DistanceUpward()
{
	// Up (0, 1, 2)
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[UP_CENTER].Position, m_Rays[UP_CENTER].Direction, m_Rays[UP_CENTER].Mask, m_RaycastDatas[UP_CENTER]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[UP_LEFT].Position, m_Rays[UP_LEFT].Direction, m_Rays[UP_LEFT].Mask, m_RaycastDatas[UP_LEFT]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[UP_RIGHT].Position, m_Rays[UP_RIGHT].Direction, m_Rays[UP_RIGHT].Mask, m_RaycastDatas[UP_RIGHT]);

	return HGF::MathUtil::Min(m_RaycastDatas[UP_CENTER].Distance, HGF::MathUtil::Min(m_RaycastDatas[UP_LEFT].Distance, m_RaycastDatas[UP_RIGHT].Distance));
}

float MapColliderComponent::DistanceDownward()
{
	// Down (3, 4, 5)
	/*
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[DOWN_CENTER].Position, m_Rays[DOWN_CENTER].Direction, m_Rays[DOWN_CENTER].Mask, m_RaycastDatas[DOWN_CENTER]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[DOWN_LEFT].Position, m_Rays[DOWN_LEFT].Direction, m_Rays[DOWN_LEFT].Mask, m_RaycastDatas[DOWN_LEFT]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[DOWN_RIGHT].Position, m_Rays[DOWN_RIGHT].Direction, m_Rays[DOWN_RIGHT].Mask, m_RaycastDatas[DOWN_RIGHT]);

	return HGF::MathUtil::Min(m_RaycastDatas[DOWN_CENTER].Distance, HGF::MathUtil::Min(m_RaycastDatas[DOWN_LEFT].Distance, m_RaycastDatas[DOWN_RIGHT].Distance));
	*/

	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[DOWN_CENTER].Position, m_Rays[DOWN_CENTER].Direction, m_Rays[DOWN_CENTER].Mask, m_RaycastDatas[DOWN_CENTER]);

	return m_RaycastDatas[DOWN_CENTER].Distance;
}

float MapColliderComponent::DistanceLeftward()
{
	// Left (6, 7, 8)
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[LEFT_CENTER].Position, m_Rays[LEFT_CENTER].Direction, m_Rays[LEFT_CENTER].Mask, m_RaycastDatas[LEFT_CENTER]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[LEFT_TOP].Position, m_Rays[LEFT_TOP].Direction, m_Rays[LEFT_TOP].Mask, m_RaycastDatas[LEFT_TOP]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[LEFT_BOTTOM].Position, m_Rays[LEFT_BOTTOM].Direction, m_Rays[LEFT_BOTTOM].Mask, m_RaycastDatas[LEFT_BOTTOM]);

	return HGF::MathUtil::Min(m_RaycastDatas[LEFT_CENTER].Distance, HGF::MathUtil::Min(m_RaycastDatas[LEFT_TOP].Distance, m_RaycastDatas[LEFT_BOTTOM].Distance));
}

float MapColliderComponent::DistanceRightward()
{
	// Right (9, 10, 11)
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[RIGHT_CENTER].Position, m_Rays[RIGHT_CENTER].Direction, m_Rays[RIGHT_CENTER].Mask, m_RaycastDatas[RIGHT_CENTER]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[RIGHT_TOP].Position, m_Rays[RIGHT_TOP].Direction, m_Rays[RIGHT_TOP].Mask, m_RaycastDatas[RIGHT_TOP]);
	m_World_TiledMap->Raycast(m_Entity_TransformComponent->GetPosition() + m_Rays[RIGHT_BOTTOM].Position, m_Rays[RIGHT_BOTTOM].Direction, m_Rays[RIGHT_BOTTOM].Mask, m_RaycastDatas[RIGHT_BOTTOM]);

	return HGF::MathUtil::Min(m_RaycastDatas[RIGHT_CENTER].Distance, HGF::MathUtil::Min(m_RaycastDatas[RIGHT_TOP].Distance, m_RaycastDatas[RIGHT_BOTTOM].Distance));
}

bool MapColliderComponent::Render(SAGE::GeometryBatch& p_GeometryBatch) const
{
	for (int i = 0; i < MaxRayCount; ++i)
	{
		p_GeometryBatch.DrawLine(m_RaycastDatas[i].StartPosition, m_RaycastDatas[i].EndPosition, HGF::Color::Yellow);
	}

	return true;
}

bool MapColliderComponent::Render(SAGE::SpriteBatch& p_SpriteBatch) const
{
	return true;
}
