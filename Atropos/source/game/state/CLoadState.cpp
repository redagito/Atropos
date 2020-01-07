#include "CLoadState.h"

#include "renderer/CRenderSystem.h"
#include "renderer/CRenderable.h"
#include "collision/CCollisionSystem.h"
#include "sound/CSoundSystem.h"
#include "game/CGameSystem.h"

#include "game/state/CPlayState.h"

#include "util/Global.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>



CLoadState::CLoadState(CRenderSystem* renderSys, CCollisionSystem* collSys, CSoundSystem* soundSys, CGameSystem* gameSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys),
	d_gameSystem(gameSys)
{
	return;
}

bool CLoadState::init()
{
	d_renderSystem->createScene("load");
	d_renderSystem->setActiveScene("load");
	
	// Load background music
	if (!d_soundSystem->loadBGM("loading", "song2.mp3", 0.5f))
	{
		return false;
	}
	return true;
}

bool CLoadState::update(float dtime)
{
	// Start game on enter
	if (glfwGetKey(Global::window, GLFW_KEY_ENTER))
	{
		d_gameSystem->removeState("play");
		if (!d_gameSystem->addState("play", new CPlayState(d_renderSystem, d_collisionSystem, d_soundSystem)))
		{
			d_nextState = "exit";
			return false;
		}
		d_nextState = "play";
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

void CLoadState::onEnter()
{
	d_renderSystem->setActiveScene("load");
	// Start sound playback
	d_soundSystem->playBGM("loading");
}

void CLoadState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& CLoadState::getNextState() const
{
	return d_nextState;
}
