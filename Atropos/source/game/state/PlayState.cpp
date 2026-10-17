#include "game/state/PlayState.h"

// Sub systems
#include "renderer/RenderSystem.h"
#include "renderer/Renderable.h"
#include "collision/CollisionSystem.h"
#include "collision/Collidable.h"
#include "sound/SoundSystem.h"

// Game object
#include "game/GameObject.h"

// Controller
#include "game/control/CameraController.h"
#include "game/control/ExplodeOnDeathController.h"
#include "game/control/LinearMovementController.h"
#include "game/control/HealthController.h"
#include "game/control/PlayerMovementController.h"
#include "game/control/ShiftController.h"
#include "game/control/WeaponController.h"
#include "game/control/DespawnController.h"
#include "game/control/RemoveOnDeathController.h"

#include "util/Global.h"
#include <spdlog/spdlog.h>

// GLFW
#include <glfw/glfw3.h>
#include <glm/ext.hpp>



PlayState::PlayState(RenderSystem* renderSys, CollisionSystem* collSys, SoundSystem* soundSys)
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

bool PlayState::init()
{
	// TODO: All resource loading and game object creation should be directed by a level file.

	// Create play scene and set as active scene
	d_renderSystem->createScene("play");
	d_renderSystem->setActiveScene("play");

	// Loading resources
	// TODO: Sound loading should be data driven, not hard coded.
	// Load laser sound
	if (!d_soundSystem->loadSFX("laser", "laser2.mp3", 0.2f))
	{
		return false;
	}

	// Load explosion sound
	if (!d_soundSystem->loadSFX("explosion", "explosion2.wav", 0.6f))
	{
		return false;
	}
	// Load background music
	if (!d_soundSystem->loadBGM("combat", "song3.mp3", 0.3f))
	{
		return false;
	}

	// Preload model resources
	if (!d_renderSystem->loadModel("shot"))
	{
		return false;
	}
	if (!d_renderSystem->loadModel("enemy"))
	{
		return false;
	}

	// Collision group ids for enemy objects and the player model
	d_playerGroup = d_collisionSystem->getNewGroupId();
	d_enemyGroup = d_collisionSystem->getNewGroupId();

	// Add player
	Renderable* playerDrawable = d_renderSystem->add("player");
	if (playerDrawable == nullptr)
	{
		SPDLOG_ERROR("Failed to add player entity: model could not be loaded.");
		return false;
	}
	// Player object controls camera
	// TODO: Player object should be deserialized from data source (xml stream?).
	d_player = new GameObject;
	d_player->setDrawable(playerDrawable);
	// Player collidable added to player collision group
	d_player->setCollidable(d_collisionSystem->add(d_player->getDrawable()->getAABBox(), d_playerGroup));
	// Sets collidable damage dealt
	d_player->getCollidable()->setDamage(30.f);
	
	// Set controller
	d_player->addController(std::shared_ptr<IController>(new CameraController(&d_renderSystem->getCamera()))); // Camera controller
	d_player->addController(std::shared_ptr<IController>(new LinearMovementController(glm::vec3(0.f, 0.f, 40.f)))); // Linear forward movement
	d_player->addController(std::shared_ptr<IController>(new PlayerMovementController(20.f))); // User controlled side movement
	d_player->addController(std::shared_ptr<IController>(new HealthController(100.f))); // Player health
	d_player->addController(std::shared_ptr<IController>(new WeaponController(d_renderSystem, d_soundSystem, d_collisionSystem, &d_world, "laser", 0.2f, "shot", d_playerGroup, 0.2f, 40.f)));

	// Set player transformation matrices
	d_player->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 5.f, 0.f)));
	d_player->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 1.f)));
	d_player->setScale(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)));

	// Add player
	d_world.addObject(d_player);

	// Generate initial water, 300 x 600, 18 tiles
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			// Adds ground tiles
			// TODO: Replace moving ground tiles with terrain system.
			// TODO: Or at least clean up the initialization, right now its huge.
			GameObject* tile = new GameObject;
			Renderable* tileDrawable = d_renderSystem->add("watertile");
			if (tileDrawable == nullptr)
			{
				SPDLOG_ERROR("Failed to add water tile entity: model could not be loaded.");
				delete tile;
				continue;
			}
			tile->setDrawable(tileDrawable);
			tile->addController(std::shared_ptr<IController>(new ShiftController(&d_renderSystem->getCamera(), 300.f)));

			// Set transformation
			tile->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(100.f * j - 300.f, 0.f, 100.f * i)));
			tile->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 1.f)));
			tile->setScale(glm::scale(glm::mat4(1.f), glm::vec3(100.f, 1.f, 100.f)));

			// Add to world
			d_world.addObject(tile);
		}
	}

	return true;
}

bool PlayState::update(float dtime)
{
	// Pause
	if (glfwGetKey(Global::window, GLFW_KEY_P))
	{
		d_nextState = "pause";
		return false;
	}
	
	// Force state transition to lose state
	if (glfwGetKey(Global::window, GLFW_KEY_F6))
	{
		d_nextState = "lose";
		return false;
	}
	
	// Force state transition to win state
	if (glfwGetKey(Global::window, GLFW_KEY_F7))
	{
		d_nextState = "win";
		return false;
	}

	// Enemy spawns about every 0.6 seconds
	d_enemySpawn -= dtime;
	if (d_enemySpawn <= 0.f)
	{
		d_enemySpawn = 0.6f;
		// Create new enemy
		GameObject* enemy = new GameObject;
		Renderable* enemyDrawable = d_renderSystem->add("enemy");
		if (enemyDrawable == nullptr)
		{
			SPDLOG_ERROR("Failed to add enemy entity: model could not be loaded.");
			delete enemy;
		}
		else
		{
			enemy->setDrawable(enemyDrawable);
			enemy->setCollidable(d_collisionSystem->add(enemy->getDrawable()->getAABBox(), d_enemyGroup));
			enemy->getCollidable()->setDamage(30.f);

			// Controller
			enemy->addController(std::shared_ptr<IController>(new LinearMovementController(glm::vec3(0.f, 0.f, -10.f))));
			enemy->addController(std::shared_ptr<IController>(new HealthController(40.f)));
			enemy->addController(std::shared_ptr<IController>(new ExplodeOnDeathController(d_soundSystem, "explosion")));
			enemy->addController(std::shared_ptr<IController>(new RemoveOnDeathController));
			enemy->addController(std::shared_ptr<IController>(new DespawnController(12.f)));

			// Transformation
			enemy->setTranslation(glm::translate(d_player->getTranslation(), glm::vec3(0.f, 0.f, 100.f)));
			enemy->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f)));
			enemy->setScale(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)));
			d_world.addObject(enemy);
		}
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

	// Game world update
	d_world.update(dtime);
	return true;
}

void PlayState::onEnter()
{
	// Set play scene as active
	// TODO: Scenes should be managed by handle, id or proxy class instead of string.
	d_renderSystem->setActiveScene("play");

	// Start sound playback
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
