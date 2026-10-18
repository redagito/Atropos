#include "game/control/HealthController.h"

#include "game/GameObject.h"
#include "collision/Collidable.h"



HealthController::HealthController(float health)
	:
	d_health(health),
	d_maxHealth(health),
	d_object(nullptr),
	d_active(true)
{
	return;
}

HealthController::~HealthController()
{
	return;
}

void HealthController::attach(GameObject* object)
{
	d_object = object;
}

void HealthController::detach()
{
	d_object = nullptr;
}

void HealthController::update(float)
{
	if (d_active && d_object != nullptr && d_object->getCollidable() != nullptr)
	{
		d_health -= d_object->getCollidable()->getDamageReceived();
		if (d_health <= 0.f)
		{
			d_object->setDead();
		}
	}
}

void HealthController::setActive(bool state)
{
	d_active = state;
}

float HealthController::getHealth() const
{
	return d_health;
}

float HealthController::getMaxHealth() const
{
	return d_maxHealth;
}