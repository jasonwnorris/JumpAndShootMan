// ProjectileComponent.hpp

#ifndef __PROJECTILECOMPONENT_HPP__
#define __PROJECTILECOMPONENT_HPP__

// Project Includes
#include "IUpdateComponent.hpp"
#include "TransformComponent.hpp"

class ProjectileComponent : public IUpdateComponent
{
	public:
		ProjectileComponent(Entity* p_Owner);
		virtual ~ProjectileComponent();

		void SetLifeTime(float p_LifeTime);
		void SetDirection(const HGF::Vector2& p_Direction);
		void SetSpeed(float p_Speed);

		virtual bool Update(float p_DeltaTime) override;

	private:
		TransformComponent* m_Entity_TransformComponent;
		float m_LifeTime;
		HGF::Vector2 m_Direction;
		float m_Speed;
};

#endif
