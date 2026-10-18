#include "game/state/LoadState.h"

#include "renderer/RenderSystem.h"
#include "renderer/scene/Camera.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/GameSystem.h"

#include "game/state/PlayState.h"


#include "util/Input.h"

#include <spdlog/spdlog.h>

#include <glfw/glfw3.h>

#include <stdexcept>

LoadState::LoadState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys, GameSystem* gameSys)
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
	d_renderSystem->setSceneBackgroundColor("load", glm::vec3(0.05f, 0.07f, 0.15f));

	// Request the assets this state and the following states need.
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
	// show() blocks until the model loads, so wait for idle or the frame freezes.
	if (!d_renderSystem->hasError() && d_renderSystem->isIdle())
	{
		d_ship.show(d_renderSystem, "player");
	}
	d_ship.update(dtime);

	// Nothing is pending once a sound has failed, so do not start music that cannot play.
	if (!d_bgmStarted && d_soundSystem->isIdle())
	{
		d_bgmStarted = true;
		if (d_soundSystem->hasError())
		{
			SPDLOG_ERROR("Not starting the loading music, a sound failed to load.");
		}
		else
		{
			d_soundSystem->playBGM("loading");
		}
	}

	const bool idle = d_renderSystem->isIdle() && d_soundSystem->isIdle();
	if (idle && !d_ready)
	{
		d_ready = true;
		SPDLOG_INFO("All resources loaded.");
	}

	if (Input::pressed(GLFW_KEY_ENTER))
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
	if (Input::pressed(GLFW_KEY_ESCAPE))
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

	// Re-entering must reset these, or the music never restarts and a stale next state leaks.
	d_bgmStarted = false;
	d_ready = false;
	d_nextState.clear();

	// The camera followed the player through the previous run.
	d_renderSystem->getCamera().resetView();
}

void LoadState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& LoadState::getNextState() const
{
	return d_nextState;
}
