// IUpdateRenderComponent.hpp

#ifndef __IUPDATERENDERCOMPONENT_HPP__
#define __IUPDATERENDERCOMPONENT_HPP__

// SAGE Includes
#include <SAGE\GeometryBatch.hpp>
#include <SAGE\SpriteBatch.hpp>
// Project Includes
#include "IComponent.hpp"

class IUpdateRenderComponent : public IComponent
{
	public:
		IUpdateRenderComponent(Entity* p_Owner);
		virtual ~IUpdateRenderComponent();

		virtual bool CanUpdate() const override;
		virtual bool CanRender() const override;

		virtual bool Update(float p_DeltaTime) = 0;
		virtual bool Render(SAGE::GeometryBatch& p_GeometryBatch) const = 0;
		virtual bool Render(SAGE::SpriteBatch& p_SpriteBatch) const = 0;
};

#endif
