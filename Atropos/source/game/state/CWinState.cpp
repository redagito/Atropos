#include "CWinState.h"

#include "renderer/CRenderSystem.h"
#include "renderer/CRenderable.h"
#include "collision/CCollisionSystem.h"
#include "sound/CSoundSystem.h"

#include "util/Global.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>



CWinState::CWinState(CRenderSystem* renderSys, CCollisionSystem* collSys, CSoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys)
{
	return;
}

bool CWinState::init()
{
	d_renderSystem->createScene("win");
	d_renderSystem->setActiveScene("win");

	// Load background music
	if (!d_soundSystem->loadBGM("win", "song1.mp3", 0.5f))
	{
		return false;
	}
	return true;
}

bool CWinState::update(float dtime)
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

void CWinState::onEnter()
{
	d_renderSystem->setActiveScene("win");
	// Start sound playback
	d_soundSystem->playBGM("win");
}

void CWinState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& CWinState::getNextState() const
{
	return d_nextState;
}
