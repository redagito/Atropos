#pragma once

#include "io/ConfigFile.h"
#include "sound/AudioDecoder.h"
#include "sound/ISoundSystem.h"
#include "core/AsyncLoader.h"
#include "core/TCreatorMixin.h"

#include <memory>
#include <string>
#include <vector>

class ThreadPool;

/**
* \brief A loaded OpenAL buffer and its playback volume.
* Deletes the OpenAL buffer on destruction, which requires a current context.
*/
struct SoundBuffer
{
	unsigned int id = 0; /**< OpenAL buffer handle. */
	float volume = 0.f;

	SoundBuffer() = default;
	SoundBuffer(const SoundBuffer&) = delete;
	SoundBuffer& operator=(const SoundBuffer&) = delete;
	~SoundBuffer();
};

/**
* \brief OpenAL sound system: decoding runs on a worker thread, the upload on the main thread in process().
*/
class SoundSystem : public TCreatorMixin<SoundSystem>, public ISoundSystem
{
public:
	SoundSystem();

	~SoundSystem();

	SoundSystem(const SoundSystem&) = delete;
	SoundSystem& operator=(const SoundSystem&) = delete;

	/**
	* \brief Initialize sound system using the given pool for decoding.
	* Throws std::runtime_error if the sound device or context cannot be created.
	*/
	void init(const ConfigFile& config, ThreadPool& pool);

	/**
	* \brief Requests an audio file as background music, loading asynchronously.
	*/
		void loadBGM(const std::string& name, const std::string& file, float volume) override;

	/**
	* \brief Requests an audio file as sound effect, loading asynchronously.
	*/
		void loadSFX(const std::string& name, const std::string& file, float volume) override;

	/**
	* \brief Play background music in loop.
	*/
		void playBGM(const std::string& name) override;

	/**
	* \brief Play sound effect once.
	*/
		void playSFX(const std::string& name) override;

		void stopAll() override;

	/**
	* \brief Commits finished sound buffers. Must run on the main thread.
	*/
		void process() override;

		bool isIdle() const override;

		bool hasError() const override;

private:
	using Loader = AsyncLoader<SoundBuffer, AudioDecoder::WaveData>;

	/**
	* \brief Decodes an audio file into PCM data.
	* Safe to run on a worker thread. Throws std::runtime_error on failure.
	*/
	static AudioDecoder::WaveData decodeAudio(const std::string& file);

	/**
	* \brief Uploads decoded PCM data to an OpenAL buffer.
	* Must run on the thread with the current OpenAL context.
	* Throws std::runtime_error if the buffer cannot be created.
	*/
	static std::unique_ptr<SoundBuffer> createBuffer(const AudioDecoder::WaveData& wave, float volume);

	std::string d_bgmPath;
	std::string d_sfxPath;
	void* d_device; /**< OpenAL device (ALCdevice). */
	void* d_context; /**< OpenAL context (ALCcontext). */
	unsigned int d_bgmSource; /**< Looping OpenAL source for background music. */
	std::vector<unsigned int> d_sfxSources; /**< Active OpenAL sources for sound effects. */
	std::unique_ptr<Loader> d_bgm;
	std::unique_ptr<Loader> d_sfx;
};
