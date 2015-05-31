// InputComponent.hpp

#ifndef __INPUTCOMPONENT_HPP__
#define __INPUTCOMPONENT_HPP__

// Project Includes
#include "TransformComponent.hpp"
#include "PlatformerMotorComponent.hpp"

class InputComponent : public IUpdateComponent
{
	public:
		InputComponent(Entity* p_Owner);
		virtual ~InputComponent();

		virtual bool Update(float p_DeltaTime) override;

	private:
		TransformComponent* m_Entity_TransformComponent;
		PlatformerMotorComponent* m_Entity_PlatformerMotorComponent;
};

#endif
