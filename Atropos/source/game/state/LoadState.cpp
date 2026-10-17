#include "game/state/LoadState.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/GameSystem.h"

#include "game/state/PlayState.h"

#include "util/Global.h"

#include <glfw/glfw3.h>



LoadState::LoadState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys, GameSystem* gameSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys),
	d_gameSystem(gameSys)
{
	return;
}

void LoadState::init()
{
	d_renderSystem->createScene("load");
	d_renderSystem->setActiveScene("load");

	// Load background music
	d_soundSystem->loadBGM("loading", "song2.mp3", 0.5f);
}

bool LoadState::update(float dtime)
{
	// Start game on enter
	if (glfwGetKey(Global::window, GLFW_KEY_ENTER))
	{
		d_gameSystem->removeState("play");
		d_gameSystem->addState("play", new PlayState(d_renderSystem, d_collisionSystem, d_soundSystem));
		d_nextState = "play";
		return false;
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
	// Start sound playback
	d_soundSystem->playBGM("loading");
}

void LoadState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& LoadState::getNextState() const
{
	return d_nextState;
}
