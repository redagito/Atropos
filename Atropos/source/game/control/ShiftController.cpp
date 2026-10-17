#include "game/control/ShiftController.h"

#include "game/GameObject.h"
#include "renderer/scene/Camera.h"

#include <glm/ext.hpp>



ShiftController::ShiftController(const Camera* camera, float shiftDistance)
	:
	d_camera(camera),
	d_shiftDistance(shiftDistance),
	d_active(true),
	d_object(nullptr)
{
	return;
}

ShiftController::~ShiftController()
{
	return;
}

void ShiftController::attach(GameObject* object)
{
	d_object = object;
}

void ShiftController::detach()
{
	d_object = nullptr;
}

void ShiftController::setActive(bool state)
{
	d_active = state;
}

void ShiftController::update(float dtime)
{
	if (d_active)
	{
		glm::vec4 temp = (d_object->getTranslation() * glm::vec4(0.f, 0.f, 0.f, 1.f));
		glm::vec3 pos = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
		if (pos.z + d_shiftDistance / 2.f < d_camera->getPosition().z)
		{
			d_object->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(pos.x, pos.y, pos.z + d_shiftDistance)));
		}
	}
}
