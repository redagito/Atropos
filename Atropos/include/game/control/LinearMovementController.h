#pragma once

#include "game/IController.h"

#include <glm/glm.hpp>

class LinearMovementController : public IController
{
public:
	LinearMovementController(const glm::vec3& velocity);

	~LinearMovementController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	glm::vec3 d_velocity; /**< Movement vector. */
	bool d_active; /**< Active state flag. */
};
