// AnimationComponent.hpp

#ifndef __ANIMATIONCOMPONENT_HPP__
#define __ANIMATIONCOMPONENT_HPP__

// HGF Includes
#include <HGF\Rectangle.hpp>
#include <HGF\Vector2.hpp>
// Project Includes
#include "IFixedUpdateComponent.hpp"
/// STL Includes
#include <map>
#include <vector>

class AnimationComponent : public IFixedUpdateComponent
{
	public:
		AnimationComponent(Entity* p_Owner);
		~AnimationComponent();

		const HGF::Rectangle& GetRegion() const;
		const HGF::Vector2& GetOrigin() const;

		bool SetState(const std::string& p_Name, const std::string& p_Value);
		bool Load(const std::string& p_Filename);

		virtual bool FixedUpdate(float p_DeltaTime) override;

	private:
		bool ChangeAnimation();

		// Change to std::map<std::string, std::vector<std::string>>?
		struct StateInfo
		{
			std::string Name;
			std::vector<std::string> Values;
		};

		struct FrameInfo
		{
			HGF::Rectangle Region;
			HGF::Vector2 Origin;
		};

		struct AnimationInfo
		{
			struct AnimationFrameInfo
			{
				int Index;
				float Time;
			};

			std::string Name;
			std::map<std::string, std::string> Conditions;
			std::vector<AnimationFrameInfo> Frames;
		};

		std::vector<StateInfo> m_StateInfoList;
		std::vector<FrameInfo> m_FrameInfoList;
		std::vector<AnimationInfo> m_AnimationInfoList;
		int m_Index;
		int m_Frame;
		std::map<std::string, std::string> m_CurrentStates;
		float m_Time;
		bool m_IsDirty;
};

#endif
