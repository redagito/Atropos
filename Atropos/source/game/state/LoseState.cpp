#include "game/state/LoseState.h"

#include "renderer/ui/UI.h"
#include "game/HUD.h"
#include "renderer/RenderSystem.h"
#include "renderer/scene/Camera.h"
#include "collision/CollisionSystem.h"
#include "sound/SoundSystem.h"
#include "game/MenuShip.h"


#include "util/Input.h"

#include <glfw/glfw3.h>

#include <glm/glm.hpp>

LoseState::LoseState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys, RunStats& runStats)
	:
d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys),
	d_runStats(runStats)
{
	return;
}

void LoseState::init()
{
	d_renderSystem->createScene("lose");
	d_renderSystem->setActiveScene("lose");
	d_renderSystem->setSceneBackgroundColor("lose", glm::vec3(0.20f, 0.05f, 0.05f));

	d_soundSystem->loadBGM("lose", "song4.mp3", 0.5f);
}

bool LoseState::update(float dtime)
{
	d_ship.update(dtime);

	UI& ui = d_renderSystem->ui();
	const float centreY = static_cast<float>(ui.getHeight());
	HUD::drawCentredText(ui, centreY * 0.3f, "YOU LOSE", 2.f, glm::vec4(1.f, 1.f, 1.f, 1.f));
	HUD::drawCentredText(ui, centreY * 0.45f, "SCORE", 1.f, glm::vec4(0.9f, 0.9f, 0.9f, 1.f));
	HUD::drawCentredText(ui, centreY * 0.55f, std::to_string(d_runStats.score), 3.f, glm::vec4(1.f, 0.85f, 0.4f, 1.f));
	HUD::drawCentredText(ui, centreY * 0.75f, "PRESS ENTER TO RETURN", 1.f, glm::vec4(1.f, 1.f, 1.f, 1.f));
	HUD::drawFps(ui);

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

void LoseState::onEnter()
{
	d_renderSystem->setActiveScene("lose");
	// The camera followed the player to where the run ended.
	d_renderSystem->getCamera().resetView();
	// Models are loaded long before a loss, so showing one does not block.
	d_ship.show(d_renderSystem, "enemy");
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
