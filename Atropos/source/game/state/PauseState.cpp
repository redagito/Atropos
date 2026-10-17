#include "game/state/PauseState.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"

#include "util/Global.h"

#include <glfw/glfw3.h>



PauseState::PauseState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys)
{
	return;
}

void PauseState::init()
{
}

bool PauseState::update(float dtime)
{
	// Continue game on enter
	if (glfwGetKey(Global::window, GLFW_KEY_ENTER))
	{
		d_nextState = "play";
		return false;
	}
	// End game on escape
	if (glfwGetKey(Global::window, GLFW_KEY_ESCAPE))
	{
		d_nextState = "exit";
		return false;
	}

	d_world.update(dtime);
	return true;
}

void PauseState::onEnter()
{
	return;
}

void PauseState::onExit()
{
	return;
}

const std::string& PauseState::getNextState() const
{
	return d_nextState;
}
