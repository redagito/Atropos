#include "game/state/WinState.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"

#include "util/Global.h"

#include <glfw/glfw3.h>



WinState::WinState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys)
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

	// Load background music
	d_soundSystem->loadBGM("win", "song1.mp3", 0.5f);
}

bool WinState::update(float dtime)
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
		d_nextState = "end";
		return false;
	}

	d_world.update(dtime);
	return true;
}

void WinState::onEnter()
{
	d_renderSystem->setActiveScene("win");
	// Start sound playback
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
