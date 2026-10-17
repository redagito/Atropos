#include "CSoundSystem.h"

// irrklang is not available as a Conan package, so the sound backend is
// disabled for the initial CMake/Conan build and the public interface is kept
// as no-ops. See the README TODO to replace irrklang with a cross-platform
// sound library.
//
//#include "irrklang/irrKlang.h"

bool CSoundSystem::init(const CConfigFile& config)
{
	d_bgmPath = config.get("sound_bgmpath", "data/sound/bgm/");
	d_sfxPath = config.get("sound_sfxpath", "data/sound/sfx/");

	// d_soundEngine = irrklang::createIrrKlangDevice();
	// return d_soundEngine != nullptr;
	return true;
}

bool CSoundSystem::loadBGM(const std::string& name, const std::string& file, float volume)
{
	// Already exists
	// if (d_bgmSources.count(name) != 0)
	// {
	// 	return true;
	// }
	// irrklang::ISoundSource* sound = d_soundEngine->addSoundSourceFromFile((d_bgmPath + file).c_str(), irrklang::ESM_AUTO_DETECT, true);
	// if (sound == nullptr)
	// {
	// 	return false;
	// }
	// sound->setDefaultVolume(volume);
	// d_bgmSources[name] = sound;
	return true;
}

bool CSoundSystem::loadSFX(const std::string& name, const std::string& file, float volume)
{
	// Already exists
	// if (d_sfxSources.count(name) != 0)
	// {
	// 	return true;
	// }
	// irrklang::ISoundSource* sound = d_soundEngine->addSoundSourceFromFile((d_sfxPath + file).c_str(), irrklang::ESM_AUTO_DETECT, true);
	// if (sound == nullptr)
	// {
	// 	return false;
	// }
	// sound->setDefaultVolume(volume);
	// d_sfxSources[name] = sound;
	return true;
}

void CSoundSystem::playBGM(const std::string& name)
{
	// assert(d_bgmSources.count(name) != 0);
	// d_soundEngine->play2D(d_bgmSources.at(name), true);
}

void CSoundSystem::playSFX(const std::string& name)
{
	// assert(d_sfxSources.count(name) != 0);
	// d_soundEngine->play2D(d_sfxSources.at(name));
}

void CSoundSystem::stopAll()
{
	// d_soundEngine->stopAllSounds();
}

CSoundSystem::CSoundSystem()
	:
	d_soundEngine(nullptr)
{
	return;
}

CSoundSystem::~CSoundSystem()
{
	// d_soundEngine->drop();
}
