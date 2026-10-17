#include "game/state/LoseState.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"

#include "util/Global.h"

#include <glfw/glfw3.h>



LoseState::LoseState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys)
{
	return;
}

void LoseState::init()
{
	d_renderSystem->createScene("lose");
	d_renderSystem->setActiveScene("lose");

	// Load background music
	d_soundSystem->loadBGM("lose", "song4.mp3", 0.5f);
}

bool LoseState::update(float dtime)
{
	// Start game on enter
	if (glfwGetKey(Global::window, GLFW_KEY_ENTER))
	{
		d_nextState = "load";
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

void LoseState::onEnter()
{
	d_renderSystem->setActiveScene("lose");
	// Start sound playback
	d_soundSystem->playBGM("lose");
}

void LoseState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& LoseState::getNextState() const
{
	return d_nextState;
}
