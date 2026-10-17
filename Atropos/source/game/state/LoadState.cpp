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
	// Showing a model blocks until it is loaded, so the display is only added
	// once the loading it would wait for is actually done. Otherwise the loading
	// screen would freeze the loop and never be seen.
	if (!d_renderSystem->hasError() && d_renderSystem->isIdle())
	{
		d_ship.show(d_renderSystem, "player");
	}
	d_ship.update(dtime);

	// Start the loading music as soon as the sounds are available. Nothing is
	// pending once a sound has failed, so the failure is reported instead of
	// starting music that will never play.
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

	// Start game on enter, but only once everything is loaded
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
	// End game on escape
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

	// A run leaves this state with the music started and the resources reported
	// as ready. Coming back has to start over, otherwise the music is never
	// played again and the stale next state can trigger an unwanted transition.
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
