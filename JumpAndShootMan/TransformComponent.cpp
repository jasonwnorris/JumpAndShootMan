// TransformComponent.cpp

// Project Includes
#include "TransformComponent.hpp"

TransformComponent::TransformComponent(Entity* p_Owner) : IComponent(p_Owner)
{
	m_Name = "TransformComponent";

	m_CurrentPosition = HGF::Vector2::Zero;
	m_CurrentRotation = 0.0f;
	m_CurrentScale = HGF::Vector2::One;
}

TransformComponent::~TransformComponent()
{
}

const HGF::Vector2& TransformComponent::GetPosition() const
{
	return m_CurrentPosition;
}

const HGF::Vector2& TransformComponent::GetPreviousPosition() const
{
	return m_PreviousPosition;
}

float TransformComponent::GetPositionX() const
{
	return m_CurrentPosition.X;
}

float TransformComponent::GetPositionY() const
{
	return m_CurrentPosition.Y;
}

void TransformComponent::SetPosition(const HGF::Vector2& p_Position)
{
	SetPositionX(p_Position.X);
	SetPositionY(p_Position.Y);
}

void TransformComponent::SetPosition(float p_PositionX, float p_PositionY)
{
	SetPositionX(p_PositionX);
	SetPositionY(p_PositionY);
}

void TransformComponent::SetPositionX(float p_PositionX)
{
	m_PreviousPosition.X = m_CurrentPosition.X;
	m_CurrentPosition.X = p_PositionX;
}

void TransformComponent::SetPositionY(float p_PositionY)
{
	m_PreviousPosition.Y = m_CurrentPosition.Y;
	m_CurrentPosition.Y = p_PositionY;
}

void TransformComponent::ResetPosition()
{
	ResetPosition(0.0f, 0.0f);
}

void TransformComponent::ResetPosition(const HGF::Vector2& p_Position)
{
	ResetPosition(p_Position.X, p_Position.Y);
}

void TransformComponent::ResetPosition(float p_PositionX, float p_PositionY)
{
	m_PreviousPosition.X = p_PositionX;
	m_PreviousPosition.Y = p_PositionY;
	m_CurrentPosition.X = p_PositionX;
	m_CurrentPosition.Y = p_PositionY;
}

void TransformComponent::Translate(const HGF::Vector2& p_Translation)
{
	TranslateX(p_Translation.X);
	TranslateY(p_Translation.Y);
}

void TransformComponent::Translate(float p_TranslationX, float p_TranslationY)
{
	TranslateX(p_TranslationX);
	TranslateY(p_TranslationY);
}

void TransformComponent::TranslateX(float p_TranslationX)
{
	m_PreviousPosition.X = m_CurrentPosition.X;
	m_CurrentPosition.X += p_TranslationX;
}

void TransformComponent::TranslateY(float p_TranslationY)
{
	m_PreviousPosition.Y = m_CurrentPosition.Y;
	m_CurrentPosition.Y += p_TranslationY;
}

const float TransformComponent::GetRotation() const
{
	return m_CurrentRotation;
}

const float TransformComponent::GetPreviousRotation() const
{
	return m_PreviousRotation;
}

void TransformComponent::SetRotation(float p_Rotation)
{
	m_PreviousRotation = m_CurrentRotation;
	m_CurrentRotation = p_Rotation;
}

void TransformComponent::ResetRotation()
{
	ResetRotation(0.0f);
}

void TransformComponent::ResetRotation(float p_Rotation)
{
	m_PreviousRotation = p_Rotation;
	m_CurrentRotation = p_Rotation;
}

void TransformComponent::Rotate(float p_Rotation)
{
	m_PreviousRotation = m_CurrentRotation;
	m_CurrentRotation += p_Rotation;
}

const HGF::Vector2& TransformComponent::GetScale() const
{
	return m_CurrentScale;
}

const HGF::Vector2& TransformComponent::GetPreviousScale() const
{
	return m_PreviousScale;
}

float TransformComponent::GetScaleX() const
{
	return m_CurrentScale.X;
}

float TransformComponent::GetScaleY() const
{
	return m_CurrentScale.Y;
}

void TransformComponent::SetScale(const HGF::Vector2& p_Scale)
{
	m_PreviousScale = m_CurrentScale;
	m_CurrentScale = p_Scale;
}

void TransformComponent::SetScale(float p_ScaleX, float p_ScaleY)
{
	SetScaleX(p_ScaleX);
	SetScaleY(p_ScaleY);
}

void TransformComponent::SetScaleX(float p_ScaleX)
{
	m_PreviousScale.X = m_CurrentScale.X;
	m_CurrentScale.X = p_ScaleX;
}

void TransformComponent::SetScaleY(float p_ScaleY)
{
	m_PreviousScale.Y = m_CurrentScale.Y;
	m_CurrentScale.Y = p_ScaleY;
}

void TransformComponent::ResetScale()
{
	ResetScale(0.0f, 0.0f);
}

void TransformComponent::ResetScale(const HGF::Vector2& p_Scale)
{
	ResetScale(p_Scale.X, p_Scale.Y);
}

void TransformComponent::ResetScale(float p_ScaleX, float p_ScaleY)
{
	m_PreviousPosition.X = p_ScaleX;
	m_PreviousPosition.Y = p_ScaleY;
	m_CurrentPosition.X = p_ScaleX;
	m_CurrentPosition.Y = p_ScaleY;
}

void TransformComponent::Scale(const HGF::Vector2& p_Scale)
{
	ScaleX(p_Scale.X);
	ScaleY(p_Scale.Y);
}

void TransformComponent::Scale(float p_ScaleX, float p_ScaleY)
{
	ScaleX(p_ScaleX);
	ScaleY(p_ScaleY);
}

void TransformComponent::ScaleX(float p_ScaleX)
{
	m_PreviousScale.X = m_CurrentScale.X;
	m_CurrentScale.X += p_ScaleX;
}

void TransformComponent::ScaleY(float p_ScaleY)
{
	m_PreviousScale.Y = m_CurrentScale.Y;
	m_CurrentScale.Y += p_ScaleY;
}
