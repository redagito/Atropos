#include "CCameraController.h"

#include "game/CGameObject.h"
#include "renderer/scene/CCamera.h"



CCameraController::CCameraController(CCamera* camera)
	:
	d_object(nullptr),
	d_camera(camera),
	d_active(true)
{
	return;
}

CCameraController::~CCameraController()
{
	return;
}

void CCameraController::attach(CGameObject* object)
{
	d_object = object;
}

void CCameraController::detach()
{
	d_object = nullptr;
	d_camera = nullptr;
}

void CCameraController::update(float dtime)
{
	if (d_active && d_object != nullptr && d_camera != nullptr)
	{
		// Update camera movement
		// Calculate player position
		glm::vec4 temp = (d_object->getTranslation() * glm::vec4(0.f, 0.f, 0.f, 1.f));
		glm::vec3 pos = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);

		// Where the camera should be
		glm::vec3 camPos(pos.x * 0.8f, pos.y * 0.7 + 5.f + std::sin(pos.z / 8.f) * 0.3, pos.z - 15.f);
		pos.z += 5.f;
		d_camera->setViewMatrix(camPos, pos, glm::vec3(0.f, 1.f, 0.f));
	}
}

void CCameraController::setActive(bool state)
{
	d_active = state;
}