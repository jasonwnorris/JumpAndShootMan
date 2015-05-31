// IRenderComponent.hpp

#ifndef __IRENDERCOMPONENT_HPP__
#define __IRENDERCOMPONENT_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// Project Includes
#include "IComponent.hpp"

class IRenderComponent : public IComponent
{
	public:
		IRenderComponent(Entity* p_Owner);
		virtual ~IRenderComponent();

		virtual bool CanRender() const override;

		virtual bool Render(SAGE::GeometryBatch& p_GeometryBatch) const = 0;
		virtual bool Render(SAGE::SpriteBatch& p_SpriteBatch) const = 0;
};

#endif
