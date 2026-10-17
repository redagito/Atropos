#include "game/state/LoadState.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/GameSystem.h"

#include "game/state/PlayState.h"

#include "util/Global.h"

#include <spdlog/spdlog.h>

#include <glfw/glfw3.h>

#include <stdexcept>



LoadState::LoadState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys, GameSystem* gameSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys),
	d_gameSystem(gameSys),
	d_bgmStarted(false),
	d_ready(false)
{
	return;
}

void LoadState::init()
{
	d_renderSystem->createScene("load");
	d_renderSystem->setActiveScene("load");

	// Request all assets for this and the following states. The loads run
	// asynchronously on the worker pool and are committed by the engine.
	d_soundSystem->loadBGM("loading", "song2.mp3", 0.5f);
	d_soundSystem->loadBGM("combat", "song3.mp3", 0.3f);
	d_soundSystem->loadSFX("laser", "laser2.mp3", 0.2f);
	d_soundSystem->loadSFX("explosion", "explosion2.wav", 0.6f);
	d_renderSystem->loadModel("player");
	d_renderSystem->loadModel("watertile");
	d_renderSystem->loadModel("shot");
	d_renderSystem->loadModel("enemy");
}

bool LoadState::update(float dtime)
{
	// Start the loading music as soon as the sounds are available.
	if (!d_bgmStarted && d_soundSystem->isIdle())
	{
		d_bgmStarted = true;
		d_soundSystem->playBGM("loading");
	}

	const bool idle = d_renderSystem->isIdle() && d_soundSystem->isIdle();
	if (idle && !d_ready)
	{
		d_ready = true;
		SPDLOG_INFO("All resources loaded.");
	}

	// Start game on enter, but only once everything is loaded
	if (glfwGetKey(Global::window, GLFW_KEY_ENTER))
	{
		if (d_renderSystem->hasError() || d_soundSystem->hasError())
		{
			throw std::runtime_error("Failed to load the game resources.");
		}
		if (!idle)
		{
			SPDLOG_INFO("Still loading resources, please wait.");
		}
		else
		{
			d_gameSystem->removeState("play");
			d_gameSystem->addState("play", new PlayState(d_renderSystem, d_collisionSystem, d_soundSystem));
			d_nextState = "play";
			return false;
		}
	}
	// End game on escape
	if (glfwGetKey(Global::window, GLFW_KEY_ESCAPE))
	{
		d_nextState = QUIT_STATE_NAME;
		return false;
	}

	d_world.update(dtime);
	return true;
}

void LoadState::onEnter()
{
	d_renderSystem->setActiveScene("load");
}

void LoadState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& LoadState::getNextState() const
{
	return d_nextState;
}
