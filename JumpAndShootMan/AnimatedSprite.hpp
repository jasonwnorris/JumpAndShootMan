// AnimatedSprite.hpp

#ifndef __ANIMATEDSPRITE_HPP__
#define __ANIMATEDSPRITE_HPP__

// HGF Includes
#include <HGF\Rectangle.hpp>
#include <HGF\Texture.hpp>
#include <HGF\Vector2.hpp>
// STL Includes
#include <map>
#include <string>
#include <vector>

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

class AnimatedSprite
{
	public:
		AnimatedSprite();
		~AnimatedSprite();

		const HGF::Texture& GetTexture() const;
		const HGF::Rectangle& GetRegion() const;
		const HGF::Vector2& GetOrigin() const;

		bool SetState(const std::string& pName, const std::string& pValue);

		bool Initialize(const std::string& pTextureFilename, const std::string& pDataFilename);
		bool Finalize();

		void Update(float pDeltaTime);

	private:
		bool ChangeAnimation();

		std::vector<StateInfo> mStateInfoList;
		std::vector<FrameInfo> mFrameInfoList;
		std::vector<AnimationInfo> mAnimationInfoList;
		HGF::Texture mTexture;
		int mIndex;
		int mFrame;
		std::map<std::string, std::string> mCurrentStates;
		float mTime;
		bool mIsDirty;
};

#endif