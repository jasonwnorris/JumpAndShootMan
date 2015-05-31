// SpriteRenderComponent.hpp

#ifndef __SPRITERENDERCOMPONENT_HPP__
#define __SPRITERENDERCOMPONENT_HPP__

// Project Includes
#include "IRenderComponent.hpp"
#include "TransformComponent.hpp"

class SpriteRenderComponent : public IRenderComponent
{
	public:
		SpriteRenderComponent(Entity* p_Owner);
		~SpriteRenderComponent();

		bool Load(const std::string& p_Filename);

		virtual bool Render(SAGE::GeometryBatch& p_GeometryBatch) const override;
		virtual bool Render(SAGE::SpriteBatch& p_SpriteBatch) const override;

	private:
		TransformComponent* m_Entity_TransformComponent;
		HGF::Texture m_Texture;
};

#endif
