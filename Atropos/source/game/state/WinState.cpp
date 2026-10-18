#include "game/state/WinState.h"

#include "renderer/RenderSystem.h"
#include "renderer/scene/Camera.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/MenuShip.h"


#include "util/Input.h"

#include <glfw/glfw3.h>

#include <glm/glm.hpp>

WinState::WinState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys)
{
	return;
}

void WinState::init()
{
	d_renderSystem->createScene("win");
	d_renderSystem->setActiveScene("win");
	d_renderSystem->setSceneBackgroundColor("win", glm::vec3(0.05f, 0.18f, 0.10f));

	d_soundSystem->loadBGM("win", "song1.mp3", 0.5f);
}

bool WinState::update(float dtime)
{
	d_ship.update(dtime);

	if (Input::pressed(GLFW_KEY_ENTER))
	{
		d_nextState = "load";
		return false;
	}
	if (Input::pressed(GLFW_KEY_ESCAPE))
	{
		d_nextState = QUIT_STATE_NAME;
		return false;
	}

	d_world.update(dtime);
	return true;
}

void WinState::onEnter()
{
	d_renderSystem->setActiveScene("win");
	// The camera followed the player to the end of the run.
	d_renderSystem->getCamera().resetView();
	// Models are loaded long before a win, so showing one does not block.
	d_ship.show(d_renderSystem, "player");
	d_soundSystem->playBGM("win");
}

void WinState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& WinState::getNextState() const
{
	return d_nextState;
}
