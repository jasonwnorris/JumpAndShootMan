// SoundManager.hpp

#ifndef __SOUNDMANAGER_HPP__
#define __SOUNDMANAGER_HPP__

#define MAX_SOUND_BUFFERS 512
#define MAX_SOUND_SOURCES 32

// AL Includes
#include <AL\al.h>
#include <AL\alc.h>
// HGF Includes
#include <HGF\Vector2.hpp>
// Project Includes
#include "ISingleton.hpp"
#include "IInitializable.hpp"
// STL Includes
#include <map>
#include <string>

class SoundManager : public ISingleton<SoundManager>, public IInitializable
{
	public:
		virtual bool Initialize() override;
		virtual bool Finalize() override;

		void SetListenerPosition(const HGF::Vector2& p_Position = HGF::Vector2::Zero);
		void SetListenerVelocity(const HGF::Vector2& p_Velocity = HGF::Vector2::Zero);

		bool Load(const std::string& p_Key, const std::string& p_Filename);
		void Play(const std::string& p_Key, const HGF::Vector2& p_Position = HGF::Vector2::Zero, const HGF::Vector2& p_Velocity = HGF::Vector2::Zero);

	private:
		const std::string GetErrorString(ALenum p_ErrorCode) const;

		std::map<std::string, int> m_AudioMap;
		ALCdevice* m_Device;
		ALCcontext* m_Context;
		int m_BufferIndex;
		unsigned int m_Buffers[MAX_SOUND_BUFFERS];
		unsigned int m_Sources[MAX_SOUND_SOURCES];
};

#endif
