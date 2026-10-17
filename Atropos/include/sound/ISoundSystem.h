#pragma once

#include <string>

/**
 * \brief Interface to the sound system.
 *
 * The game only ever asks for a clip to be loaded or played, which is what this
 * exposes. Everything the concrete system needs to get there, such as opening a
 * device or handing work to a thread pool, stays outside the interface.
 */
class ISoundSystem
{
public:
	virtual ~ISoundSystem() = default;

	/**
	* \brief Requests a background track.
	*/
	virtual void loadBGM(const std::string& name, const std::string& file, float volume) = 0;

	/**
	* \brief Requests a sound effect.
	*/
	virtual void loadSFX(const std::string& name, const std::string& file, float volume) = 0;

	/**
	* \brief Plays a previously requested background track.
	*/
	virtual void playBGM(const std::string& name) = 0;

	/**
	* \brief Plays a previously requested sound effect.
	*/
	virtual void playSFX(const std::string& name) = 0;

	/**
	* \brief Stops all playback.
	*/
	virtual void stopAll() = 0;

	/**
	* \brief Commits finished audio resources. Must run on the main thread.
	*/
	virtual void process() = 0;

	/**
	* \brief True when all requested audio is ready or failed.
	*/
	virtual bool isIdle() const = 0;

	/**
	* \brief True if any requested audio failed to load.
	*/
	virtual bool hasError() const = 0;
};