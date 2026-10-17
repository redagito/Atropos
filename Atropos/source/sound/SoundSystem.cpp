#include "sound/SoundSystem.h"

#include "sound/AudioDecoder.h"
#include "debug/Log.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <cstddef>

SoundSystem::SoundSystem()
	:
	d_device(nullptr),
	d_context(nullptr),
	d_bgmSource(0)
{
	return;
}

SoundSystem::~SoundSystem()
{
	if (d_context != nullptr)
	{
		stopAll();

		if (d_bgmSource != 0)
		{
			alDeleteSources(1, &d_bgmSource);
			d_bgmSource = 0;
		}

		for (const auto& entry : d_bgmBuffers)
		{
			ALuint buffer = entry.second.buffer;
			alDeleteBuffers(1, &buffer);
		}

		for (const auto& entry : d_sfxBuffers)
		{
			ALuint buffer = entry.second.buffer;
			alDeleteBuffers(1, &buffer);
		}

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(static_cast<ALCcontext*>(d_context));
		d_context = nullptr;
	}

	if (d_device != nullptr)
	{
		alcCloseDevice(static_cast<ALCdevice*>(d_device));
		d_device = nullptr;
	}
}

bool SoundSystem::init(const ConfigFile& config)
{
	d_bgmPath = config.get("sound_bgmpath", "data/sound/bgm/");
	d_sfxPath = config.get("sound_sfxpath", "data/sound/sfx/");

	ALCdevice* device = alcOpenDevice(nullptr);
	if (device == nullptr)
	{
		return false;
	}

	ALCcontext* context = alcCreateContext(device, nullptr);
	if (context == nullptr)
	{
		alcCloseDevice(device);
		return false;
	}

	if (alcMakeContextCurrent(context) == ALC_FALSE)
	{
		alcDestroyContext(context);
		alcCloseDevice(device);
		return false;
	}

	alGenSources(1, &d_bgmSource);
	if (d_bgmSource == 0 || alGetError() != AL_NO_ERROR)
	{
		if (d_bgmSource != 0)
		{
			alDeleteSources(1, &d_bgmSource);
			d_bgmSource = 0;
		}
		alcDestroyContext(context);
		alcCloseDevice(device);
		return false;
	}
	alSourcei(d_bgmSource, AL_LOOPING, AL_TRUE);

	d_device = device;
	d_context = context;
	return true;
}

bool SoundSystem::load(const std::string& name, const std::string& file, float volume, const std::string& path, std::unordered_map<std::string, Buffer>& buffers)
{
	if (buffers.count(name) != 0)
	{
		return true;
	}

	AudioDecoder::WaveData wave;
	if (!AudioDecoder::decodeFile(path + file, wave))
	{
		return false;
	}

	if (wave.channels < 1 || wave.channels > 2)
	{
		LOG_ERROR("Unsupported channel count %d for %s.", wave.channels, name.c_str());
		return false;
	}

	ALuint buffer = 0;
	alGenBuffers(1, &buffer);
	if (buffer == 0)
	{
		LOG_ERROR("Failed to generate OpenAL buffer for %s.", name.c_str());
		return false;
	}
	const std::size_t byteCount = wave.samples.size() * sizeof(short);
	alBufferData(buffer, wave.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		wave.samples.data(), static_cast<ALsizei>(byteCount), wave.sampleRate);
	if (alGetError() != AL_NO_ERROR)
	{
		LOG_ERROR("Failed to upload audio data for %s.", name.c_str());
		alDeleteBuffers(1, &buffer);
		return false;
	}

	Buffer entry;
	entry.buffer = buffer;
	entry.volume = volume;
	buffers[name] = entry;
	return true;
}

bool SoundSystem::loadBGM(const std::string& name, const std::string& file, float volume)
{
	return load(name, file, volume, d_bgmPath, d_bgmBuffers);
}

bool SoundSystem::loadSFX(const std::string& name, const std::string& file, float volume)
{
	return load(name, file, volume, d_sfxPath, d_sfxBuffers);
}

void SoundSystem::playBGM(const std::string& name)
{
	const auto it = d_bgmBuffers.find(name);
	if (it == d_bgmBuffers.end())
	{
		return;
	}

	alSourceStop(d_bgmSource);
	alSourcei(d_bgmSource, AL_BUFFER, static_cast<ALint>(it->second.buffer));
	alSourcef(d_bgmSource, AL_GAIN, it->second.volume);
	alSourcePlay(d_bgmSource);
}

void SoundSystem::playSFX(const std::string& name)
{
	const auto it = d_sfxBuffers.find(name);
	if (it == d_sfxBuffers.end())
	{
		return;
	}

	for (auto source = d_sfxSources.begin(); source != d_sfxSources.end();)
	{
		ALint state = AL_STOPPED;
		alGetSourcei(*source, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
		{
			ALuint handle = *source;
			alDeleteSources(1, &handle);
			source = d_sfxSources.erase(source);
		}
		else
		{
			++source;
		}
	}

	ALuint handle = 0;
	alGenSources(1, &handle);
	if (handle == 0)
	{
		return;
	}
	alSourcei(handle, AL_BUFFER, static_cast<ALint>(it->second.buffer));
	alSourcef(handle, AL_GAIN, it->second.volume);
	alSourcePlay(handle);
	d_sfxSources.push_back(handle);
}

void SoundSystem::stopAll()
{
	if (d_context == nullptr)
	{
		return;
	}

	if (d_bgmSource != 0)
	{
		alSourceStop(d_bgmSource);
	}

	for (const auto& source : d_sfxSources)
	{
		ALuint handle = source;
		alDeleteSources(1, &handle);
	}
	d_sfxSources.clear();
}
