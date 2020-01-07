#include "CShiftController.h"

#include "game/CGameObject.h"
#include "renderer/scene/CCamera.h"

#include <glm/ext.hpp>



CShiftController::CShiftController(const CCamera* camera, float shiftDistance)
	:
	d_camera(camera),
	d_shiftDistance(shiftDistance),
	d_active(true),
	d_object(nullptr)
{
	return;
}

CShiftController::~CShiftController()
{
	return;
}

void CShiftController::attach(CGameObject* object)
{
	d_object = object;
}

void CShiftController::detach()
{
	d_object = nullptr;
}

void CShiftController::setActive(bool state)
{
	d_active = state;
}

void CShiftController::update(float dtime)
{
	if (d_active)
	{
		glm::vec4 temp = (d_object->getTranslation() * glm::vec4(0.f, 0.f, 0.f, 1.f));
		glm::vec3 pos = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
		if (pos.z + d_shiftDistance / 2.f < d_camera->getPosition().z)
		{
			d_object->setTranslation(glm::translate(glm::vec3(pos.x, pos.y, pos.z + d_shiftDistance)));
		}
	}
}
