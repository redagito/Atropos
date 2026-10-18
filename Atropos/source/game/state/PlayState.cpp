#include "game/state/PlayState.h"

#include "renderer/RenderSystem.h"
#include "renderer/Renderable.h"
#include "collision/CollisionSystem.h"
#include "collision/Collidable.h"
#include "sound/SoundSystem.h"

#include "game/GameObject.h"

#include "game/control/CameraController.h"
#include "game/control/ExplodeOnDeathController.h"
#include "game/control/LinearMovementController.h"
#include "game/control/HealthController.h"
#include "game/control/PlayerMovementController.h"
#include "game/control/ShiftController.h"
#include "game/control/WeaponController.h"
#include "game/control/DespawnController.h"
#include "game/control/RemoveOnDeathController.h"


#include "util/Input.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>

PlayState::PlayState(IRenderSystem* renderSys, CollisionSystem* collSys, ISoundSystem* soundSys)
	:
	d_renderSystem(renderSys),
	d_collisionSystem(collSys),
	d_soundSystem(soundSys),
	d_player(nullptr),
	d_playerGroup(0),
	d_enemyGroup(0),
	d_enemySpawn(2.f)
{
	return;
}

PlayState::~PlayState()
{
	// Clear objects before releasing the groups, or collidables outlive their groups.
	d_world.clear();
	d_collisionSystem->releaseGroup(d_playerGroup);
	d_collisionSystem->releaseGroup(d_enemyGroup);
}

void PlayState::init()
{
	// TODO: All resource loading and game object creation should be directed by a level file.

	d_renderSystem->createScene("play");
	d_renderSystem->setActiveScene("play");

	// TODO: Sound loading should be data driven, not hard coded.
	d_soundSystem->loadSFX("laser", "laser2.mp3", 0.2f);
	d_soundSystem->loadSFX("explosion", "explosion2.wav", 0.6f);
	d_soundSystem->loadBGM("combat", "song3.mp3", 0.3f);

	d_renderSystem->loadModel("shot");
	d_renderSystem->loadModel("enemy");

	d_playerGroup = d_collisionSystem->getNewGroupId();
	d_enemyGroup = d_collisionSystem->getNewGroupId();

	Renderable* playerDrawable = d_renderSystem->add("player");
	// TODO: Player object should be deserialized from data source (xml stream?).
	d_player = new GameObject;
	d_player->setDrawable(playerDrawable);
	d_player->setCollidable(d_collisionSystem->add(d_player->getDrawable()->getAABBox(), d_playerGroup));
	d_player->getCollidable()->setDamage(30.f);
	
	d_player->addController(std::shared_ptr<IController>(new CameraController(&d_renderSystem->getCamera()))); // Camera controller
	d_player->addController(std::shared_ptr<IController>(new LinearMovementController(glm::vec3(0.f, 0.f, 40.f)))); // Linear forward movement
	d_player->addController(std::shared_ptr<IController>(new PlayerMovementController(20.f))); // User controlled side movement
	d_player->addController(std::shared_ptr<IController>(new HealthController(100.f))); // Player health
	d_player->addController(std::shared_ptr<IController>(new WeaponController(d_renderSystem, d_soundSystem, d_collisionSystem, &d_world, "laser", 0.2f, "shot", d_playerGroup, 0.2f, 40.f)));

	d_player->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 5.f, 0.f)));
	d_player->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 1.f)));
	d_player->setScale(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)));

	d_world.addObject(d_player);

	// Ground tiles: 3 x 6 grid, 600 x 300 units.
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			// TODO: Replace the hard coded ground tile grid with a terrain system.
			Renderable* tileDrawable = d_renderSystem->add("watertile");
			GameObject* tile = new GameObject;
			tile->setDrawable(tileDrawable);
			tile->addController(std::shared_ptr<IController>(new ShiftController(&d_renderSystem->getCamera(), 300.f)));

			tile->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(100.f * j - 300.f, 0.f, 100.f * i)));
			tile->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 1.f)));
			tile->setScale(glm::scale(glm::mat4(1.f), glm::vec3(100.f, 1.f, 100.f)));

			d_world.addObject(tile);
		}
	}
}

bool PlayState::update(float dtime)
{
	if (Input::pressed(GLFW_KEY_P))
	{
		d_nextState = "pause";
		return false;
	}
	
	if (Input::pressed(GLFW_KEY_F6))
	{
		d_nextState = "lose";
		return false;
	}
	
	if (Input::pressed(GLFW_KEY_F7))
	{
		d_nextState = "win";
		return false;
	}

	d_enemySpawn -= dtime;
	if (d_enemySpawn <= 0.f)
	{
		d_enemySpawn = 0.6f;
		Renderable* enemyDrawable = d_renderSystem->add("enemy");
		GameObject* enemy = new GameObject;
		enemy->setDrawable(enemyDrawable);
		enemy->setCollidable(d_collisionSystem->add(enemy->getDrawable()->getAABBox(), d_enemyGroup));
		enemy->getCollidable()->setDamage(30.f);

		enemy->addController(std::shared_ptr<IController>(new LinearMovementController(glm::vec3(0.f, 0.f, -10.f))));
		// Health is below any single hit, so one contact kills the enemy.
	enemy->addController(std::shared_ptr<IController>(new HealthController(25.f)));
		enemy->addController(std::shared_ptr<IController>(new ExplodeOnDeathController(d_soundSystem, "explosion")));
		enemy->addController(std::shared_ptr<IController>(new RemoveOnDeathController));
		enemy->addController(std::shared_ptr<IController>(new DespawnController(12.f)));

		enemy->setTranslation(glm::translate(d_player->getTranslation(), glm::vec3(0.f, 0.f, 100.f)));
		enemy->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f)));
		enemy->setScale(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)));
		d_world.addObject(enemy);
	}

	// Game end conditions. Player dies or survives long enough.
	if (d_player->isDead())
	{
		d_nextState = "lose";
		return false;
	}
	else if (d_renderSystem->getCamera().getPosition().z >= 8000.f)
	{
		d_nextState = "win";
		return false;
	}

	d_world.update(dtime);
	return true;
}

void PlayState::onEnter()
{
	// TODO: Scenes should be managed by handle, id or proxy class instead of string.
	d_renderSystem->setActiveScene("play");

	d_soundSystem->playBGM("combat");
}

void PlayState::onExit()
{
	d_soundSystem->stopAll();
}

const std::string& PlayState::getNextState() const
{
	return d_nextState;
}
