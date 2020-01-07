#include "CPauseState.h"

#include "renderer/CRenderSystem.h"
#include "renderer/CRenderable.h"
#include "collision/CCollisionSystem.h"
#include "sound/CSoundSystem.h"

#include "util/Global.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>



CPauseState::CPauseState(CRenderSystem* renderSys, CCollisionSystem* collSys, CSoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys)
{
	return;
}

bool CPauseState::init()
{
	return true;
}

bool CPauseState::update(float dtime)
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

void CPauseState::onEnter()
{
	return;
}

void CPauseState::onExit()
{
	return;
}

const std::string& CPauseState::getNextState() const
{
	return d_nextState;
}
