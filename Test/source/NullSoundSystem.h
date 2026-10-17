#pragma once

#include "sound/ISoundSystem.h"

#include <string>
#include <vector>

/**
 * \brief Sound system test double that records what the game asked for.
 *
 * Mirrors the public API of ISoundSystem. There is no audio device behind it,
 * so playback is a no-op and the calls are recorded instead.
 */
class NullSoundSystem : public ISoundSystem
{
public:
	void loadBGM(const std::string& name, const std::string&, float) override
	{
		d_bgmLoads.push_back(name);
	}

	void loadSFX(const std::string& name, const std::string&, float) override
	{
		d_sfxLoads.push_back(name);
	}

	void playBGM(const std::string& name) override
	{
		d_bgmPlays.push_back(name);
	}

	void playSFX(const std::string& name) override
	{
		d_sfxPlays.push_back(name);
	}

	void stopAll() override
	{
		++d_stopAllCount;
	}

	void process() override
	{
	}

	bool isIdle() const override
	{
		return true;
	}

	bool hasError() const override
	{
		return false;
	}

	std::vector<std::string> d_bgmLoads; /**< Names of background tracks requested. */
	std::vector<std::string> d_sfxLoads; /**< Names of sound effects requested. */
	std::vector<std::string> d_bgmPlays; /**< Names of background tracks played, in order. */
	std::vector<std::string> d_sfxPlays; /**< Names of sound effects played, in order. */
	int d_stopAllCount = 0; /**< How often playback was stopped. */
};