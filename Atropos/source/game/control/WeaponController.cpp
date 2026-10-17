#include "game/control/WeaponController.h"

#include "game/GameObject.h"
#include "game/GameWorld.h"
#include "sound/SoundSystem.h"
#include "renderer/RenderSystem.h"
#include "collision/CollisionSystem.h"
#include "collision/Collidable.h"

// Controller
#include "game/control/LinearMovementController.h"
#include "game/control/HealthController.h"
#include "game/control/RemoveOnDeathController.h"
#include "game/control/DespawnController.h"

#include "util/Global.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>



WeaponController::WeaponController(RenderSystem* renderSys, SoundSystem* soundSys, CollisionSystem* collSys, GameWorld* world, const std::string& soundName, float volume, const std::string& modelName, unsigned int collisionGroup, float cooldown, float damage)
	:
	d_object(nullptr),
	d_renderSys(renderSys),
	d_soundSys(soundSys),
	d_collSys(collSys),
	d_world(world),
	d_soundName(soundName),
	d_modelName(modelName),
	d_damage(damage),
	d_volume(volume),
	d_cooldown(cooldown),
	d_currentCooldown(0.f),
	d_collisionGroup(collisionGroup),
	d_active(true)
{
	return;
}

WeaponController::~WeaponController()
{
	return;
}

void WeaponController::attach(GameObject* object)
{
	d_object = object;
}

void WeaponController::detach()
{
	d_object = nullptr;
}

void WeaponController::update(float dtime)
{
	if (d_active && d_object != nullptr)
	{
		// Update timer
		d_currentCooldown -= dtime;
		// TODO: Input management should be done in separate system.
		if (glfwGetKey(Global::window, GLFW_KEY_SPACE))
		{
			// Check cooldown
			if (d_currentCooldown <= 0.f)
			{
				// Reset timer
				d_currentCooldown = d_cooldown;

				// Create new projectile
				Renderable* projectileDrawable = d_renderSys->add(d_modelName.c_str());
				GameObject* projectile = new GameObject;
				projectile->setDrawable(projectileDrawable);
				projectile->getDrawable()->setShadow(false);
				projectile->setCollidable(d_collSys->add(projectile->getDrawable()->getAABBox(), d_collisionGroup));
				projectile->getCollidable()->setDamage(d_damage);

				// Controller
				projectile->addController(std::shared_ptr<IController>(new LinearMovementController(glm::vec3(0.f, 0.f, 100.f))));
				projectile->addController(std::shared_ptr<IController>(new HealthController(0.1f)));
				projectile->addController(std::shared_ptr<IController>(new RemoveOnDeathController));
				projectile->addController(std::shared_ptr<IController>(new DespawnController(1.5f)));

				// Transformation
				projectile->setTranslation(glm::translate(d_object->getTranslation(), glm::vec3(0.f, 0.f, 2.f)));
				projectile->setRotation(glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 1.f)));
				projectile->setScale(glm::scale(glm::mat4(1.f), glm::vec3(3.f, 3.f, 3.f)));

				// Add to world
				d_world->addObject(projectile);

				// Play shot sound
				d_soundSys->playSFX(d_soundName);
			}
		}
	}
}

void WeaponController::setActive(bool state)
{
	d_active = state;
}
