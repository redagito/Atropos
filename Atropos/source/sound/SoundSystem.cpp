#include "sound/SoundSystem.h"

#include "sound/AudioDecoder.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <spdlog/spdlog.h>

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

SoundBuffer::~SoundBuffer()
{
	if (id != 0)
	{
		ALuint buffer = id;
		alDeleteBuffers(1, &buffer);
	}
}

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
	// Release buffers while the context is current; the thread pool must outlive this object.
	d_bgm = nullptr;
	d_sfx = nullptr;

	if (d_context != nullptr)
	{
		stopAll();

		if (d_bgmSource != 0)
		{
			alDeleteSources(1, &d_bgmSource);
			d_bgmSource = 0;
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

void SoundSystem::init(const ConfigFile& config, ThreadPool& pool)
{
	d_bgmPath = config.get("sound_bgmpath", "data/sound/bgm/");
	d_sfxPath = config.get("sound_sfxpath", "data/sound/sfx/");

	ALCdevice* device = alcOpenDevice(nullptr);
	if (device == nullptr)
	{
		throw std::runtime_error("Failed to open the OpenAL device.");
	}

	ALCcontext* context = alcCreateContext(device, nullptr);
	if (context == nullptr)
	{
		alcCloseDevice(device);
		throw std::runtime_error("Failed to create the OpenAL context.");
	}

	if (alcMakeContextCurrent(context) == ALC_FALSE)
	{
		alcDestroyContext(context);
		alcCloseDevice(device);
		throw std::runtime_error("Failed to activate the OpenAL context.");
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
		throw std::runtime_error("Failed to generate the background music source.");
	}
	alSourcei(d_bgmSource, AL_LOOPING, AL_TRUE);

	d_device = device;
	d_context = context;

	d_bgm = std::make_unique<Loader>(pool);
	d_sfx = std::make_unique<Loader>(pool);
}

AudioDecoder::WaveData SoundSystem::decodeAudio(const std::string& file)
{
	AudioDecoder::WaveData wave;
	if (!AudioDecoder::decodeFile(file, wave))
	{
		throw std::runtime_error("Failed to decode audio file: " + file);
	}

	if (wave.channels < 1 || wave.channels > 2)
	{
		throw std::runtime_error("Unsupported channel count in audio file: " + file);
	}

	return wave;
}

std::unique_ptr<SoundBuffer> SoundSystem::createBuffer(const AudioDecoder::WaveData& wave, float volume)
{
	ALuint buffer = 0;
	alGenBuffers(1, &buffer);
	if (buffer == 0)
	{
		throw std::runtime_error("Failed to generate an OpenAL buffer.");
	}
	const std::size_t byteCount = wave.samples.size() * sizeof(short);
	alBufferData(buffer, wave.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		wave.samples.data(), static_cast<ALsizei>(byteCount), wave.sampleRate);
	if (alGetError() != AL_NO_ERROR)
	{
		alDeleteBuffers(1, &buffer);
		throw std::runtime_error("Failed to upload audio data.");
	}

	auto sound = std::make_unique<SoundBuffer>();
	sound->id = buffer;
	sound->volume = volume;
	return sound;
}

void SoundSystem::loadBGM(const std::string& name, const std::string& file, float volume)
{
	const std::string path = d_bgmPath + file;
	d_bgm->request(name,
		[path] { return decodeAudio(path); },
		[volume](const AudioDecoder::WaveData& wave) { return createBuffer(wave, volume); });
}

void SoundSystem::loadSFX(const std::string& name, const std::string& file, float volume)
{
	const std::string path = d_sfxPath + file;
	d_sfx->request(name,
		[path] { return decodeAudio(path); },
		[volume](const AudioDecoder::WaveData& wave) { return createBuffer(wave, volume); });
}

void SoundSystem::playBGM(const std::string& name)
{
	const ResourceHandle<SoundBuffer> sound = d_bgm->getHandle(name);
	// Skip a missing, loading or failed track instead of letting the slot throw.
	if (!sound.isReady())
	{
		SPDLOG_ERROR("Background music {} is not ready to play.", name);
		return;
	}
	SoundBuffer* buffer = sound.get();

	alSourceStop(d_bgmSource);
	alSourcei(d_bgmSource, AL_BUFFER, static_cast<ALint>(buffer->id));
	alSourcef(d_bgmSource, AL_GAIN, buffer->volume);
	alSourcePlay(d_bgmSource);
}

void SoundSystem::playSFX(const std::string& name)
{
	const ResourceHandle<SoundBuffer> sound = d_sfx->getHandle(name);
	if (!sound.isReady())
	{
		SPDLOG_ERROR("Sound effect {} is not ready to play.", name);
		return;
	}
	SoundBuffer* buffer = sound.get();

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
		throw std::runtime_error("Failed to generate an OpenAL source for: " + name);
	}
	alSourcei(handle, AL_BUFFER, static_cast<ALint>(buffer->id));
	alSourcef(handle, AL_GAIN, buffer->volume);
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

void SoundSystem::process()
{
	d_bgm->process();
	d_sfx->process();
}

bool SoundSystem::isIdle() const
{
	return d_bgm->isIdle() && d_sfx->isIdle();
}

bool SoundSystem::hasError() const
{
	return d_bgm->hasError() || d_sfx->hasError();
}
