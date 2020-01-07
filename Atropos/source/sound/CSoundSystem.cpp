#include "CSoundSystem.h"

#include "irrklang/irrKlang.h"

#include <cassert>

bool CSoundSystem::init(const CConfigFile& config)
{
	d_bgmPath = config.get("sound_bgmpath", "data/sound/bgm/");
	d_sfxPath = config.get("sound_sfxpath", "data/sound/sfx/");
	d_soundEngine = irrklang::createIrrKlangDevice();
	return d_soundEngine != nullptr;
}

bool CSoundSystem::loadBGM(const std::string& name, const std::string& file, float volume)
{
	// Already exists
	if (d_bgmSources.count(name) != 0)
	{
		return true;
	}
	irrklang::ISoundSource* sound = d_soundEngine->addSoundSourceFromFile((d_bgmPath + file).c_str(), irrklang::ESM_AUTO_DETECT, true);
	if (sound == nullptr)
	{
		return false;
	}
	sound->setDefaultVolume(volume);
	d_bgmSources[name] = sound;
	return true;
}

bool CSoundSystem::loadSFX(const std::string& name, const std::string& file, float volume)
{
	// Already exists
	if (d_sfxSources.count(name) != 0)
	{
		return true;
	}
	irrklang::ISoundSource* sound = d_soundEngine->addSoundSourceFromFile((d_sfxPath + file).c_str(), irrklang::ESM_AUTO_DETECT, true);
	if (sound == nullptr)
	{
		return false;
	}
	sound->setDefaultVolume(volume);
	d_sfxSources[name] = sound;
	return true;
}

void CSoundSystem::playBGM(const std::string& name)
{
	assert(d_bgmSources.count(name) != 0);
	d_soundEngine->play2D(d_bgmSources.at(name), true);
}

void CSoundSystem::playSFX(const std::string& name)
{
	assert(d_sfxSources.count(name) != 0);
	d_soundEngine->play2D(d_sfxSources.at(name));
}

void CSoundSystem::stopAll()
{
	d_soundEngine->stopAllSounds();
}

CSoundSystem::CSoundSystem()
	:
	d_soundEngine(nullptr)
{
	return;
}

CSoundSystem::~CSoundSystem()
{
	d_soundEngine->drop();
}