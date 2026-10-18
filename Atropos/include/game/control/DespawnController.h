#pragma once

#include "game/IController.h"

#include <string>

class DespawnController : public IController
{
public:
	DespawnController(float despawnTime);

	~DespawnController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_despawnTime;
	bool d_active;
};
