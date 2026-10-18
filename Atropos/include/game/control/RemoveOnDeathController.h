#pragma once

#include "game/IController.h"

#include <string>

// TODO: Should react to an onDeath event instead of polling isDead().
class RemoveOnDeathController : public IController
{
public:
	RemoveOnDeathController();

	~RemoveOnDeathController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	bool d_active;
};
