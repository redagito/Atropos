#include "CWeaponController.h"

#include "game/CGameObject.h"
#include "game/CGameWorld.h"
#include "sound/CSoundSystem.h"
#include "renderer/CRenderSystem.h"
#include "collision/CCollisionSystem.h"
#include "collision/CCollidable.h"

// Controller
#include "CLinearMovementController.h"
#include "CHealthController.h"
#include "CRemoveOnDeathController.h"
#include "CDespawnController.h"

#include "util/Global.h"

#include <glfw/glfw3.h>
#include <glm/ext.hpp>



CWeaponController::CWeaponController(CRenderSystem* renderSys, CSoundSystem* soundSys, CCollisionSystem* collSys, CGameWorld* world, const std::string& soundName, float volume, const std::string& modelName, unsigned int collisionGroup, float cooldown, float damage)
	:
	d_object(nullptr),
	d_active(true),
	d_renderSys(renderSys),
	d_soundSys(soundSys),
	d_collSys(collSys),
	d_world(world),
	d_soundName(soundName),
	d_volume(volume),
	d_modelName(modelName),
	d_cooldown(cooldown),
	d_currentCooldown(0.f),
	d_damage(damage),
	d_collisionGroup(collisionGroup)
{
	return;
}

CWeaponController::~CWeaponController()
{
	return;
}

void CWeaponController::attach(CGameObject* object)
{
	d_object = object;
}

void CWeaponController::detach()
{
	d_object = nullptr;
}

void CWeaponController::update(float dtime)
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
				CGameObject* projectile = new CGameObject;
				projectile->setDrawable(d_renderSys->add(d_modelName.c_str(), "shot"));
				projectile->getDrawable()->setShadow(false);
				projectile->setCollidable(d_collSys->add(projectile->getDrawable()->getAABBox(), d_collisionGroup));
				projectile->getCollidable()->setDamage(d_damage);

				// Controller
				projectile->addController(std::shared_ptr<IController>(new CLinearMovementController(glm::vec3(0.f, 0.f, 100.f))));
				projectile->addController(std::shared_ptr<IController>(new CHealthController(0.1f)));
				projectile->addController(std::shared_ptr<IController>(new CRemoveOnDeathController));
				projectile->addController(std::shared_ptr<IController>(new CDespawnController(1.5f)));

				// Transformation
				projectile->setTranslation(glm::translate(d_object->getTranslation(), glm::vec3(0.f, 0.f, 2.f)));
				projectile->setRotation(glm::rotate(0.f, glm::vec3(1.f, 1.f, 1.f)));
				projectile->setScale(glm::scale(glm::vec3(3.f, 3.f, 3.f)));

				// Add to world
				d_world->addObject(projectile);

				// Play shot sound
				d_soundSys->playSFX(d_soundName);
			}
		}
	}
}

void CWeaponController::setActive(bool state)
{
	d_active = state;
}
