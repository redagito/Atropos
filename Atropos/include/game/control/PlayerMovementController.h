#pragma once

#include "game/IController.h"

class PlayerMovementController : public IController
{
public:
	PlayerMovementController(float speedSide);

	~PlayerMovementController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_speedSide; /**< Side movement speed. */
	float d_rotationDegree; /**< Current rotation in degree. */
	bool d_active; /**< Active state flag. */
};
