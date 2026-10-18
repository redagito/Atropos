#include "game/control/DespawnController.h"

#include "game/GameObject.h"



DespawnController::DespawnController(float despawnTime)
	:
	d_object(nullptr),
	d_active(true),
	d_despawnTime(despawnTime)
{
	return;
}

DespawnController::~DespawnController()
{
	return;
}

void DespawnController::attach(GameObject* object)
{
	d_object = object;
}

void DespawnController::detach()
{
	d_object = nullptr;
}

void DespawnController::update(float dtime)
{
	if (d_object != nullptr && d_active)
	{
		d_despawnTime -= dtime;
		if (d_despawnTime <= 0.f)
		{
			d_object->markDeleted();
		}
	}
}

void DespawnController::setActive(bool state)
{
	d_active = state;
}
