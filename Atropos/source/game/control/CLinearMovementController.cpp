#include "CLinearMovementController.h"

#include "game/CGameObject.h"
#include "collision/CCollidable.h"

#include <glm/ext.hpp>



CLinearMovementController::CLinearMovementController(const glm::vec3& velocity)
	:
	d_velocity(velocity),
	d_active(true),
	d_object(nullptr)
{
	return;
}

CLinearMovementController::~CLinearMovementController()
{
	return;
}

void CLinearMovementController::attach(CGameObject* object)
{
	d_object = object;
}

void CLinearMovementController::detach()
{
	d_object = nullptr;
}

void CLinearMovementController::setActive(bool state)
{
	d_active = state;
}

void CLinearMovementController::update(float dtime)
{
	if (d_active)
	{
		d_object->setTranslation(glm::translate(d_object->getTranslation(), d_velocity * dtime));
	}
}