#pragma once

class Camera;

#include "game/IController.h"

#include <glm/glm.hpp>

class ShiftController : public IController
{
public:
	ShiftController(const Camera* camera, float shiftDistance);

	~ShiftController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_shiftDistance; /**< Shifting distance. */
	const Camera* d_camera; /**< Active camera object. */
	bool d_active; /**< Active state flag. */
};
