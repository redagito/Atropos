#include "game/control/LinearMovementController.h"

#include "game/GameObject.h"

#include <glm/ext.hpp>



LinearMovementController::LinearMovementController(const glm::vec3& velocity)
	:
	d_velocity(velocity),
	d_active(true),
	d_object(nullptr)
{
	return;
}

LinearMovementController::~LinearMovementController()
{
	return;
}

void LinearMovementController::attach(GameObject* object)
{
	d_object = object;
}

void LinearMovementController::detach()
{
	d_object = nullptr;
}

void LinearMovementController::setActive(bool state)
{
	d_active = state;
}

void LinearMovementController::update(float dtime)
{
	if (d_active && d_object != nullptr)
	{
		d_object->setTranslation(glm::translate(d_object->getTranslation(), d_velocity * dtime));
	}
}