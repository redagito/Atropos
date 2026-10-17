#pragma once

#include "io/ConfigFile.h"
#include "core/TCreatorMixin.h"

#include <string>
#include <unordered_map>
#include <vector>

/**
* \brief Sound system class.
* Wraps and manages the OpenAL sound device and sound buffers.
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
	* \brief Initialize sound system.
	* Throws std::runtime_error if the sound device or context cannot be created.
	*/
	void init(const ConfigFile& config);

	/**
	* \brief Load audio file as background music.
	* Throws std::runtime_error if the file cannot be loaded.
	*/
	void loadBGM(const std::string& name, const std::string& file, float volume);

	/**
	* \brief Load audio file as sound effect.
	* Throws std::runtime_error if the file cannot be loaded.
	*/
	void loadSFX(const std::string& name, const std::string& file, float volume);

	/**
	* \brief Play background music in loop.
	*/
	void playBGM(const std::string& name);

	/**
	* \brief Play sound effect once.
	*/
	void playSFX(const std::string& name);

	/**
	* \brief Stops all sound playback.
	*/
	void stopAll();

private:
	struct Buffer
	{
		unsigned int buffer; /**< OpenAL buffer handle. */
		float volume; /**< Playback volume. */
	};

	void load(const std::string& name, const std::string& file, float volume, const std::string& path, std::unordered_map<std::string, Buffer>& buffers);

	std::string d_bgmPath; /**< Background music path. */
	std::string d_sfxPath; /**< Sound effects path. */
	void* d_device; /**< OpenAL device (ALCdevice). */
	void* d_context; /**< OpenAL context (ALCcontext). */
	unsigned int d_bgmSource; /**< Looping OpenAL source for background music. */
	std::unordered_map<std::string, Buffer> d_bgmBuffers; /**< Background music buffers. */
	std::unordered_map<std::string, Buffer> d_sfxBuffers; /**< Sound effect buffers. */
	std::vector<unsigned int> d_sfxSources; /**< Active OpenAL sources for sound effects. */
};
