#include "CLoseState.h"

#include "renderer/CRenderSystem.h"
#include "renderer/CRenderable.h"
#include "collision/CCollisionSystem.h"
#include "sound/CSoundSystem.h"

#include "util/Global.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>



CLoseState::CLoseState(CRenderSystem* renderSys, CCollisionSystem* collSys, CSoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys)
{
	return;
}

bool CLoseState::init()
{
	d_renderSystem->createScene("lose");
	d_renderSystem->setActiveScene("lose");

	// Load background music
	if (!d_soundSystem->loadBGM("lose", "song4.mp3", 0.5f))
	{
		return false;
	}
	return true;
}

bool CLoseState::update(float dtime)
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

void CLoseState::onEnter()
{
	d_renderSystem->setActiveScene("lose");
	// Start sound playback
	d_soundSystem->playBGM("lose");
}

void CLoseState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& CLoseState::getNextState() const
{
	return d_nextState;
}
