// IUpdateComponent.hpp

#ifndef __IUPDATECOMPONENT_HPP__
#define __IUPDATECOMPONENT_HPP__

// Project Includes
#include "IComponent.hpp"

class IUpdateComponent : public IComponent
{
	public:
		IUpdateComponent(Entity* p_Owner);
		virtual ~IUpdateComponent();

		virtual bool CanUpdate() const override;

		virtual bool Update(float p_DeltaTime) = 0;
};

#endif
