#pragma once

#include <string>

class ISoundSystem
{
public:
	virtual ~ISoundSystem() = default;

	virtual void loadBGM(const std::string& name, const std::string& file, float volume) = 0;

	virtual void loadSFX(const std::string& name, const std::string& file, float volume) = 0;

	/**
	* \brief Plays a previously requested background track.
	*/
	virtual void playBGM(const std::string& name) = 0;

	/**
	* \brief Plays a previously requested sound effect.
	*/
	virtual void playSFX(const std::string& name) = 0;

	virtual void stopAll() = 0;

	/**
	* \brief Commits finished audio resources. Must run on the main thread.
	*/
	virtual void process() = 0;

	/**
	* \brief True when every requested clip is ready or has failed.
	*/
	virtual bool isIdle() const = 0;

	virtual bool hasError() const = 0;
};