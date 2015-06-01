// SpriteRenderComponent.cpp

// Project Includes
#include "Entity.hpp"
#include "AnimationComponent.hpp"
#include "PlatformerMotorComponent.hpp"
#include "SpriteRenderComponent.hpp"
#include "TextureManager.hpp"
// JsonCpp Includes
#include <json\json.h>
// STL Includes
#include <algorithm>
#include <fstream>

SpriteRenderComponent::SpriteRenderComponent(Entity* p_Owner) : IRenderComponent(p_Owner)
{
	m_Name = "SpriteRenderComponent";

	m_Entity_TransformComponent = m_Owner->GetComponent<TransformComponent>();
}

SpriteRenderComponent::~SpriteRenderComponent()
{
	m_Entity_TransformComponent = nullptr;
}

bool SpriteRenderComponent::Load(const std::string& p_Filename)
{
	m_Texture = TextureManager::Load(p_Filename, HGF::Interpolation::Nearest, HGF::Wrapping::ClampToEdge);

	return m_Texture != nullptr;
}

bool SpriteRenderComponent::Render(SAGE::GeometryBatch& p_GeometryBatch) const
{
	const HGF::Vector2& position = m_Entity_TransformComponent->GetPosition();
	const HGF::Vector2& scale = m_Entity_TransformComponent->GetScale();
	int width = 0;
	int height = 0;

	// TODO: Decouple from AnimationComponent.
	AnimationComponent* ac = m_Owner->GetComponent<AnimationComponent>();
	if (ac != nullptr)
	{
		const HGF::Rectangle& region = ac->GetRegion();
		width = region.Width;
		height = region.Height;
	}
	else
	{
		width = m_Texture->GetWidth();
		height = m_Texture->GetHeight();
	}

	p_GeometryBatch.DrawRectangle(HGF::Vector2(position.X - width / 2.0f * scale.X, position.Y - height / 2.0f * scale.Y),
		HGF::Vector2(position.X + width / 2.0f * scale.X, position.Y + height / 2.0f * scale.Y),
		HGF::Color::Green);

	return true;
}

bool SpriteRenderComponent::Render(SAGE::SpriteBatch& p_SpriteBatch) const
{
	const HGF::Vector2& position = m_Entity_TransformComponent->GetPosition();
	const HGF::Vector2& scale = m_Entity_TransformComponent->GetScale();
	HGF::Rectangle region;
	HGF::Vector2 origin;

	// TODO: Decouple from AnimationComponent.
	AnimationComponent* ac = m_Owner->GetComponent<AnimationComponent>();
	if (ac != nullptr)
	{
		region = ac->GetRegion();
		origin = ac->GetOrigin();
	}
	else
	{
		region.Width = m_Texture->GetWidth();
		region.Height = m_Texture->GetHeight();
		origin.X = 0.0f;
		origin.Y = 0.0f;
	}

	// TODO: Remove dependence on PlatformerMotorComponent.
	PlatformerMotorComponent* pmc = m_Owner->GetComponent<PlatformerMotorComponent>();
	if (pmc != nullptr)
	{
		p_SpriteBatch.Draw(*m_Texture,
			position,
			region,
			HGF::Color::White,
			origin,
			0.0f,
			scale,
			pmc->IsFacingLeft() ? SAGE::Orientation::FlipHorizontal : SAGE::Orientation::None);
	}
	else
	{
		p_SpriteBatch.Draw(*m_Texture,
			position,
			region,
			HGF::Color::White,
			origin,
			0.0f,
			scale,
			SAGE::Orientation::None);
	}

	return true;
}
