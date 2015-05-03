// AnimatedSprite.cpp

// Project Includes
#include "AnimatedSprite.hpp"
// JsonCpp Includes
#include <json\json.h>
// STL Includes
#include <algorithm>
#include <iostream>
#include <fstream>

AnimatedSprite::AnimatedSprite()
{
	mIndex = 0;
	mFrame = 0;
	mTime = 0.0f;
	mIsDirty = false;
}

AnimatedSprite::~AnimatedSprite()
{
}

const HGF::Texture& AnimatedSprite::GetTexture() const
{
	return mTexture;
}

const HGF::Rectangle& AnimatedSprite::GetRegion() const
{
	return mFrameInfoList[mAnimationInfoList[mIndex].Frames[mFrame].Index].Region;
}

const HGF::Vector2& AnimatedSprite::GetOrigin() const
{
	return mFrameInfoList[mAnimationInfoList[mIndex].Frames[mFrame].Index].Origin;
}

bool AnimatedSprite::SetState(const std::string& pName, const std::string& pValue)
{
	// Determine if the provided combination is valid.
	// (This is gross and I should probably change it.)
	auto iter = std::find_if(mStateInfoList.begin(), mStateInfoList.end(), [pName, pValue](const StateInfo& pStateInfo) {
		return pStateInfo.Name == pName && std::find(pStateInfo.Values.begin(), pStateInfo.Values.end(), pValue) != pStateInfo.Values.end();
	});

	// Change the animation if it's a different, valid state.
	if (iter != mStateInfoList.end())
	{
		if (mCurrentStates[pName] != pValue)
		{
			mCurrentStates[pName] = pValue;
			mIsDirty = true;

			return true;
		}
	}

	return false;
}

bool AnimatedSprite::Initialize(const std::string& pTextureFilename, const std::string& pDataFilename)
{
	if (!mTexture.Load(pTextureFilename, HGF::Interpolation::Nearest, HGF::Wrapping::ClampToEdge))
		return false;

	Uint32 start = SDL_GetTicks();

	bool success = true;

	// Load configuration file.
	std::ifstream file(pDataFilename);
	if (file.is_open())
	{
		std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		Json::Value data;
		Json::Reader reader;
		if (reader.parse(contents, data))
		{
			//Json::StyledWriter writer;
			//std::cout << writer.write(root) << std::endl;

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

					mStateInfoList.push_back(stateInfo);
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

					mFrameInfoList.push_back(frameInfo);
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

					mAnimationInfoList.push_back(animationInfo);
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

	Uint32 end = SDL_GetTicks();

	std::cout << "Read JSON file '" << pDataFilename << "' in " << (end - start) << " milliseconds." << std::endl;

	return success;
}

bool AnimatedSprite::Finalize()
{
	return true;
}

void AnimatedSprite::Update(float pDeltaTime)
{
	if (mIsDirty)
	{
		ChangeAnimation();
	}
	else
	{
		mTime += pDeltaTime;
	}

	if (mTime > mAnimationInfoList[mIndex].Frames[mFrame].Time)
	{
		mFrame = (mFrame + 1) % mAnimationInfoList[mIndex].Frames.size();
		mTime = 0.0f;
	}
}

bool AnimatedSprite::ChangeAnimation()
{
	for (int i = 0; i < (int)mAnimationInfoList.size(); ++i)
	{
		if (mAnimationInfoList[i].Conditions == mCurrentStates)
		{
			mIndex = i;
			mFrame = 0;
			mTime = 0.0f;
			mIsDirty = false;

			return true;
		}
	}

	return false;
}