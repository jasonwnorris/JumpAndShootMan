// SoundManager.cpp

// SDL Includes
#include <SDL2\SDL_mixer.h>
// HGF Includes
#include <HGF\Console.hpp>
// Project Includes
#include "SoundManager.hpp"

bool SoundManager::Initialize()
{
	// Initialize SDL_Mixer.
	int flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;
	if (Mix_Init(flags) < 0)
	{
		return false;
	}

	// Open SDL_Mixer.
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0)
	{
		return false;
	}

	// Open the OpenAL device.
	m_Device = alcOpenDevice(nullptr);
	if (m_Device == nullptr)
	{
		return false;
	}

	// Create the OpenAL context.
	m_Context = alcCreateContext(m_Device, nullptr);
	if (m_Context == nullptr)
	{
		return false;
	}

	// Make the OpenAL context current.
	alcMakeContextCurrent(m_Context);

	// Allocate memory.
	alGenBuffers(MAX_SOUND_BUFFERS, m_Buffers);
	alGenSources(MAX_SOUND_SOURCES, m_Sources);

	// Zero out listener.
	static ALfloat ori[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, ori);

	// Initialize others.
	m_BufferIndex = 0;

	return true;
}

bool SoundManager::Finalize()
{
	// Clear mapping.
	m_AudioMap.clear();

	// Delete memory.
	alDeleteBuffers(MAX_SOUND_BUFFERS, m_Buffers);
	alDeleteSources(MAX_SOUND_SOURCES, m_Sources);

	// Close everything.
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_Context);
	alcCloseDevice(m_Device);
	Mix_CloseAudio();
	Mix_Quit();

	return true;
}

void SoundManager::SetListenerPosition(const HGF::Vector2& p_Position)
{
	alListener3f(AL_POSITION, p_Position.X, p_Position.Y, 0.0f);
}

void SoundManager::SetListenerVelocity(const HGF::Vector2& p_Velocity)
{
	alListener3f(AL_VELOCITY, p_Velocity.X, p_Velocity.Y, 0.0f);
}

bool SoundManager::Load(const std::string& p_Key, const std::string& p_Filename)
{
	// Check if already loaded.
	std::map<std::string, int>::iterator iter = m_AudioMap.find(p_Key);
	if (iter != m_AudioMap.end())
	{
		return true;
	}

	// Load audio WAV file.
	Mix_Chunk* chunk = Mix_LoadWAV(p_Filename.c_str());
	if (chunk == NULL)
	{
		HGF::Console::WriteLine("[SoundManager::Load] Failed to load audio file: '%s', %s", p_Filename, Mix_GetError());

		return false;
	}
	
	// Insert data into buffer.
	alBufferData(m_Buffers[m_BufferIndex], AL_FORMAT_MONO16, chunk->abuf, chunk->alen, 44100);

	// Free the audio WAV file.
	Mix_FreeChunk(chunk);

	// Insert into mapping.
	m_AudioMap[p_Key] = m_BufferIndex;

	// Step to next index.
	++m_BufferIndex;

	return true;
}

void SoundManager::Play(const std::string& p_Key, const HGF::Vector2& p_Position, const HGF::Vector2& p_Velocity)
{
	static ALfloat pitch = 1.0f;
	static ALfloat gain = 0.25f;
	static ALfloat ori[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	static ALboolean looping = AL_FALSE;

	// Check if exists.
	std::map<std::string, int>::iterator iter = m_AudioMap.find(p_Key);
	if (iter != m_AudioMap.end())
	{
		// Find an open source.
		for (int i = 0; i < MAX_SOUND_SOURCES; ++i)
		{
			ALint value;
			alGetSourcei(m_Sources[i], AL_SOURCE_STATE, &value);
			if (value != AL_PLAYING)
			{
				// Set values and play.
				alSource3f(m_Sources[i], AL_POSITION, p_Position.X, p_Position.Y, 0.0f);
				alSource3f(m_Sources[i], AL_VELOCITY, p_Velocity.X, p_Velocity.Y, 0.0f);
				alSourcef(m_Sources[i], AL_PITCH, pitch);
				alSourcef(m_Sources[i], AL_GAIN, gain);
				alSourcefv(m_Sources[i], AL_ORIENTATION, ori);
				alSourcei(m_Sources[i], AL_LOOPING, looping);
				alSourcei(m_Sources[i], AL_BUFFER, m_Buffers[iter->second]);
				alSourcePlay(m_Sources[i]);

				return;
			}
		}
	}
}

const std::string SoundManager::GetErrorString(ALenum p_ErrorCode) const
{
	switch (p_ErrorCode)
	{
		case AL_NO_ERROR:
			return "OpenAL No Error";
		case AL_INVALID_NAME:
			return "OpenAL Invalid Name";
		case AL_INVALID_ENUM:
			return "OpenAL Invalid Enumeration";
		case AL_INVALID_VALUE:
			return "OpenAL Invalid Value";
		case AL_INVALID_OPERATION:
			return "OpenAL Invalid Operation";
		case AL_OUT_OF_MEMORY:
			return "OpenAL Out of Memory";
		default:
			return "Unknown error type.";
	}
}
