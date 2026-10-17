#pragma once

#include "io/ConfigFile.h"
#include "sound/AudioDecoder.h"
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
	float volume = 0.f; /**< Playback volume. */

	SoundBuffer() = default;
	SoundBuffer(const SoundBuffer&) = delete;
	SoundBuffer& operator=(const SoundBuffer&) = delete;
	~SoundBuffer();
};

/**
* \brief Sound system class.
* Wraps and manages the OpenAL sound device and sound buffers.
* Files are decoded on a worker thread, the OpenAL upload runs on the main
* thread during process().
*/
class SoundSystem : public TCreatorMixin<SoundSystem>
{
public:
	/**
	* \brief Constructer.
	*/
	SoundSystem();

	/**
	* \brief Destructor.
	*/
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
	void loadBGM(const std::string& name, const std::string& file, float volume);

	/**
	* \brief Requests an audio file as sound effect, loading asynchronously.
	*/
	void loadSFX(const std::string& name, const std::string& file, float volume);

	/**
	* \brief Play background music in loop.
	* Throws std::runtime_error if the sound failed to load and std::logic_error
	* if it is not ready yet.
	*/
	void playBGM(const std::string& name);

	/**
	* \brief Play sound effect once.
	* Throws std::runtime_error if the sound failed to load and std::logic_error
	* if it is not ready yet.
	*/
	void playSFX(const std::string& name);

	/**
	* \brief Stops all sound playback.
	*/
	void stopAll();

	/**
	* \brief Commits finished sound buffers. Must run on the main thread.
	*/
	void process();

	/**
	* \brief True when all requested sounds are loaded or failed.
	*/
	bool isIdle() const;

	/**
	* \brief True if any requested sound failed to load.
	*/
	bool hasError() const;

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

	std::string d_bgmPath; /**< Background music path. */
	std::string d_sfxPath; /**< Sound effects path. */
	void* d_device; /**< OpenAL device (ALCdevice). */
	void* d_context; /**< OpenAL context (ALCcontext). */
	unsigned int d_bgmSource; /**< Looping OpenAL source for background music. */
	std::vector<unsigned int> d_sfxSources; /**< Active OpenAL sources for sound effects. */
	std::unique_ptr<Loader> d_bgm; /**< Background music buffers. */
	std::unique_ptr<Loader> d_sfx; /**< Sound effect buffers. */
};
