#pragma once

namespace irrklang
{
class ISoundEngine; /**< Irrklang sound engine forward declare. */
class ISoundSource; /**< Irrklang sound source forward declare. */
}

#include "io/CConfigFile.h"
#include "core/TCreatorMixin.h"

#include <string>
#include <unordered_map>
#include <memory>

/**
* \brief Sound system class.
* Wraps and manages irrklang sound engine and sound sources.
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
	std::string d_bgmPath; /**< Background music path. */
	std::string d_sfxPath; /**< Sound effects path. */
	irrklang::ISoundEngine* d_soundEngine; /**< Sound engine. */
	std::unordered_map<std::string, irrklang::ISoundSource*> d_sfxSources; /**< Sound effect sources. */
	std::unordered_map<std::string, irrklang::ISoundSource*> d_bgmSources; /**< Background music sources. */
};
