// AnimationComponent.cpp

// Project Includes
#include "AnimationComponent.hpp"
// JsonCpp Includes
#include <json\json.h>
// STL Includes
#include <algorithm>
#include <fstream>

AnimationComponent::AnimationComponent(Entity* p_Owner) : IFixedUpdateComponent(p_Owner)
{
	m_Name = "AnimationComponent";
}

AnimationComponent::~AnimationComponent()
{
}

const HGF::Rectangle& AnimationComponent::GetRegion() const
{
	return m_FrameInfoList[m_AnimationInfoList[m_Index].Frames[m_Frame].Index].Region;
}

const HGF::Vector2& AnimationComponent::GetOrigin() const
{
	return m_FrameInfoList[m_AnimationInfoList[m_Index].Frames[m_Frame].Index].Origin;
}

bool AnimationComponent::SetState(const std::string& p_Name, const std::string& p_Value)
{
	// Determine if the provided combination is valid.
	// (This is gross and I should probably change it.)
	auto iter = std::find_if(m_StateInfoList.begin(), m_StateInfoList.end(), [p_Name, p_Value](const StateInfo& pStateInfo) {
		return pStateInfo.Name == p_Name && std::find(pStateInfo.Values.begin(), pStateInfo.Values.end(), p_Value) != pStateInfo.Values.end();
	});

	// Change the animation if it's a different, valid state.
	if (iter != m_StateInfoList.end())
	{
		if (m_CurrentStates[p_Name] != p_Value)
		{
			m_CurrentStates[p_Name] = p_Value;
			m_IsDirty = true;

			return true;
		}
	}

	return false;
}

bool AnimationComponent::Load(const std::string& p_Filename)
{
	bool success = true;

	// Load configuration file.
	std::ifstream file(p_Filename);
	if (file.is_open())
	{
		std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		Json::Value data;
		Json::Reader reader;
		if (reader.parse(contents, data))
		{
			// Read state information.
			Json::Value states = data.get("States", Json::Value::null);
			if (states != Json::Value::null)
			{
				for (int i = 0; i < (int)states.size(); ++i)
				{
					StateInfo stateInfo;
					stateInfo.Name = states[i].get("Name", "Unknown State").asString();

					// Read state values.
					Json::Value values = states[i].get("Values", Json::Value::null);
					if (values != Json::Value::null)
					{
						for (int j = 0; j < (int)values.size(); ++j)
						{
							stateInfo.Values.push_back(values[j].asString());
						}
					}

					m_StateInfoList.push_back(stateInfo);
				}
			}

			// Read frame information.
			Json::Value frames = data.get("Frames", Json::Value::null);
			if (frames != Json::Value::null)
			{
				for (int i = 0; i < (int)frames.size(); ++i)
				{
					FrameInfo frameInfo;

					// Read frame origin.
					Json::Value origin = frames[i].get("Origin", Json::Value::null);
					if (origin != Json::Value::null)
					{
						frameInfo.Origin.X = (float)origin.get("X", 0).asDouble();
						frameInfo.Origin.Y = (float)origin.get("Y", 0).asDouble();
					}

					// Read frame region.
					Json::Value region = frames[i].get("Region", Json::Value::null);
					if (region != Json::Value::null)
					{
						frameInfo.Region.X = region.get("X", 0).asInt();
						frameInfo.Region.Y = region.get("Y", 0).asInt();
						frameInfo.Region.Width = region.get("Width", 0).asInt();
						frameInfo.Region.Height = region.get("Height", 0).asInt();
					}

					m_FrameInfoList.push_back(frameInfo);
				}
			}

			// Read animation information.
			Json::Value animations = data.get("Animations", Json::Value::null);
			if (animations != Json::Value::null)
			{
				for (int i = 0; i < (int)animations.size(); ++i)
				{
					AnimationInfo animationInfo;
					animationInfo.Name = animations[i].get("Name", "Unknown Animation").asString();

					// Read animation conditions.
					Json::Value conditions = animations[i].get("Conditions", Json::Value::null);
					if (conditions != Json::Value::null)
					{
						for (int j = 0; j < (int)conditions.size(); ++j)
						{
							std::string key = conditions[j].get("Key", "Unknown Key").asString();
							std::string value = conditions[j].get("Value", "Unknown Value").asString();

							animationInfo.Conditions[key] = value;
						}
					}

					// Read animation frames.
					Json::Value frames = animations[i].get("Frames", Json::Value::null);
					if (frames != Json::Value::null)
					{
						for (int j = 0; j < (int)frames.size(); ++j)
						{
							AnimationInfo::AnimationFrameInfo animationFrameInfo;
							animationFrameInfo.Index = frames[j].get("Index", 0).asInt();
							animationFrameInfo.Time = (float)frames[j].get("Time", 0).asDouble();

							animationInfo.Frames.push_back(animationFrameInfo);
						}
					}

					m_AnimationInfoList.push_back(animationInfo);
				}

			}
		}
		else
		{
			success = false;
		}
	}
	else
	{
		success = false;
	}
	file.close();

	return success;
}

bool AnimationComponent::FixedUpdate(float p_DeltaTime)
{
	if (m_IsDirty)
	{
		ChangeAnimation();
	}
	else
	{
		m_Time += p_DeltaTime;
	}

	if (m_Time > m_AnimationInfoList[m_Index].Frames[m_Frame].Time)
	{
		m_Frame = (m_Frame + 1) % m_AnimationInfoList[m_Index].Frames.size();
		m_Time = 0.0f;
	}

	return true;
}

bool AnimationComponent::ChangeAnimation()
{
	for (int i = 0; i < (int)m_AnimationInfoList.size(); ++i)
	{
		if (m_AnimationInfoList[i].Conditions == m_CurrentStates)
		{
			m_Index = i;
			m_Frame = 0;
			m_Time = 0.0f;
			m_IsDirty = false;

			return true;
		}
	}

	return false;
}
