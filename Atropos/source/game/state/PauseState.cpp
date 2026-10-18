#include "game/state/PauseState.h"

#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"


#include "util/Input.h"

#include <glfw/glfw3.h>

PauseState::PauseState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys)
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
	if (Input::pressed(GLFW_KEY_ENTER))
	{
		d_nextState = "play";
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

bool PauseState::collisionsEnabled() const
{
	return false;
}
