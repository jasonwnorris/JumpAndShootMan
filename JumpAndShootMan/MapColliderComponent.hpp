// MapColliderComponent.hpp

#ifndef __MAPCOLLIDERCOMPONENT_HPP__
#define __MAPCOLLIDERCOMPONENT_HPP__

// Project Includes
#include "IRenderComponent.hpp"
#include "TransformComponent.hpp"
#include "TiledMap.hpp"

class MapColliderComponent : public IRenderComponent
{
	public:
		MapColliderComponent(Entity* p_Owner);
		~MapColliderComponent();

		float DistanceUpward();
		float DistanceDownward();
		float DistanceLeftward();
		float DistanceRightward();

		virtual bool Render(SAGE::GeometryBatch& p_GeometryBatch) const override;
		virtual bool Render(SAGE::SpriteBatch& p_SpriteBatch) const override;

	private:
		static const int MaxRayCount = 12;
		enum RayIndex
		{
			UP_CENTER,
			UP_LEFT,
			UP_RIGHT,
			DOWN_CENTER,
			DOWN_LEFT,
			DOWN_RIGHT,
			LEFT_CENTER,
			LEFT_TOP,
			LEFT_BOTTOM,
			RIGHT_CENTER,
			RIGHT_TOP,
			RIGHT_BOTTOM,
		};

		TransformComponent* m_Entity_TransformComponent;
		const TiledMap* m_World_TiledMap;
		float m_UpHitDistance;
		float m_DownHitDistance;
		float m_LeftHitDistance;
		float m_RightHitDistance;
		Ray m_Rays[12];
		RaycastData m_RaycastDatas[12];
};

#endif
