#pragma once

#include "io/CConfigFile.h"
#include "core/TCreatorMixin.h"

#include <string>
#include <unordered_map>
#include <vector>

/**
* \brief Sound system class.
* Wraps and manages the OpenAL sound device and sound buffers.
*/
class CSoundSystem : public TCreatorMixin<CSoundSystem>
{
public:
	/**
	* \brief Constructer.
	*/
	CSoundSystem();

	/**
	* \brief Destructor.
	*/
	~CSoundSystem();

	/**
	* \brief Initialize sound system.
	*/
	bool init(const CConfigFile& config);

	/**
	* \brief Load audio file as background music.
	*/
	bool loadBGM(const std::string& name, const std::string& file, float volume);

	/**
	* \brief Load audio file as sound effect.
	*/
	bool loadSFX(const std::string& name, const std::string& file, float volume);

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
	struct SBuffer
	{
		unsigned int buffer; /**< OpenAL buffer handle. */
		float volume; /**< Playback volume. */
	};

	bool load(const std::string& name, const std::string& file, float volume, const std::string& path, std::unordered_map<std::string, SBuffer>& buffers);

	std::string d_bgmPath; /**< Background music path. */
	std::string d_sfxPath; /**< Sound effects path. */
	void* d_device; /**< OpenAL device (ALCdevice). */
	void* d_context; /**< OpenAL context (ALCcontext). */
	unsigned int d_bgmSource; /**< Looping OpenAL source for background music. */
	std::unordered_map<std::string, SBuffer> d_bgmBuffers; /**< Background music buffers. */
	std::unordered_map<std::string, SBuffer> d_sfxBuffers; /**< Sound effect buffers. */
	std::vector<unsigned int> d_sfxSources; /**< Active OpenAL sources for sound effects. */
};
