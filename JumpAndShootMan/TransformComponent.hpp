// TransformComponent.hpp

#ifndef __TRANSFORMCOMPONENT_HPP__
#define __TRANSFORMCOMPONENT_HPP__

// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "IComponent.hpp"

class TransformComponent : public IComponent
{
	public:
		TransformComponent(Entity* p_Owner);
		~TransformComponent();

		// Position
		const HGF::Vector2& GetPosition() const;
		const HGF::Vector2& GetPreviousPosition() const;
		float GetPositionX() const;
		float GetPositionY() const;
		void SetPosition(const HGF::Vector2& p_Position);
		void SetPosition(float p_PositionX, float p_PositionY);
		void SetPositionX(float p_PositionX);
		void SetPositionY(float p_PositionY);
		void ResetPosition();
		void ResetPosition(const HGF::Vector2& p_Position);
		void ResetPosition(float p_PositionX, float p_PositionY);
		void Translate(const HGF::Vector2& p_Translation);
		void Translate(float p_TranslationX, float p_TranslationY);
		void TranslateX(float p_TranslationX);
		void TranslateY(float p_TranslationY);

		// Rotation
		const float GetRotation() const;
		const float GetPreviousRotation() const;
		void SetRotation(float p_Rotation);
		void ResetRotation();
		void ResetRotation(float p_Rotation);
		void Rotate(float p_Rotation);

		// Scale
		const HGF::Vector2& GetScale() const;
		const HGF::Vector2& GetPreviousScale() const;
		float GetScaleX() const;
		float GetScaleY() const;
		void SetScale(const HGF::Vector2& p_Scale);
		void SetScale(float p_ScaleX, float p_ScaleY);
		void SetScaleX(float p_ScaleX);
		void SetScaleY(float p_ScaleY);
		void ResetScale();
		void ResetScale(const HGF::Vector2& p_Scale);
		void ResetScale(float p_ScaleX, float p_ScaleY);
		void Scale(const HGF::Vector2& p_Scale);
		void Scale(float p_ScaleX, float p_ScaleY);
		void ScaleX(float p_ScaleX);
		void ScaleY(float p_ScaleY);

	private:
		HGF::Vector2 m_CurrentPosition;
		HGF::Vector2 m_PreviousPosition;
		float m_CurrentRotation;
		float m_PreviousRotation;
		HGF::Vector2 m_CurrentScale;
		HGF::Vector2 m_PreviousScale;
};

#endif
