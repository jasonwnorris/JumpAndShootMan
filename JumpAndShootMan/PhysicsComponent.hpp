// PhysicsComponent.hpp

#ifndef __PHYSICSCOMPONENT_HPP__
#define __PHYSICSCOMPONENT_HPP__

// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "IUpdateComponent.hpp"
#include "TransformComponent.hpp"

class PhysicsComponent : public IUpdateComponent
{
	public:
		PhysicsComponent(Entity* p_Owner);
		~PhysicsComponent();

		virtual bool Update(float p_DeltaTime) override;

		// Velocity
		const HGF::Vector2& GetVelocity() const;
		const HGF::Vector2& GetPreviousVelocity() const;
		float GetVelocityX() const;
		float GetVelocityY() const;
		void SetVelocity(const HGF::Vector2& p_Velocity);
		void SetVelocity(float p_VelocityX, float p_VelocityY);
		void SetVelocityX(float p_VelocityX);
		void SetVelocityY(float p_VelocityY);
		void ResetVelocity();
		void ResetVelocity(const HGF::Vector2& p_Velocity);
		void ResetVelocity(float p_VelocityX, float p_VelocityY);

		// Acceleration
		const HGF::Vector2& GetAcceleration() const;
		const HGF::Vector2& GetPreviousAcceleration() const;
		float GetAccelerationX() const;
		float GetAccelerationY() const;
		void SetAcceleration(const HGF::Vector2& p_Acceleration);
		void SetAcceleration(float p_AccelerationX, float p_AccelerationY);
		void SetAccelerationX(float p_AccelerationX);
		void SetAccelerationY(float p_AccelerationY);
		void ResetAcceleration();
		void ResetAcceleration(const HGF::Vector2& p_Acceleration);
		void ResetAcceleration(float p_AccelerationX, float p_AccelerationY);

	private:
		TransformComponent* m_Entity_TransformComponent;
		HGF::Vector2 m_CurrentVelocity;
		HGF::Vector2 m_PreviousVelocity;
		HGF::Vector2 m_CurrentAcceleration;
		HGF::Vector2 m_PreviousAcceleration;
};

#endif
