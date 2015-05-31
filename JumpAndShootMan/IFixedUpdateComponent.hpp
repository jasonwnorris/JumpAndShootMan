// IFixedUpdateComponent.hpp

#ifndef __IFIXEDUPDATECOMPONENT_HPP__
#define __IFIXEDUPDATECOMPONENT_HPP__

// Project Includes
#include "IUpdateComponent.hpp"

class IFixedUpdateComponent : public IUpdateComponent
{
	public:
		IFixedUpdateComponent(Entity* p_Owner);
		virtual ~IFixedUpdateComponent();

		bool Update(float p_DeltaTime) override final;
		virtual bool FixedUpdate(float p_TimeStep) = 0;

	private:
		float m_Accumulator;
		float m_TimeStep;
};

#endif
