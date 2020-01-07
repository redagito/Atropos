#include "CDespawnController.h"

#include "game/CGameObject.h"



CDespawnController::CDespawnController(float despawnTime)
	:
	d_object(nullptr),
	d_active(true),
	d_despawnTime(despawnTime)
{
	return;
}

CDespawnController::~CDespawnController()
{
	return;
}

void CDespawnController::attach(CGameObject* object)
{
	d_object = object;
}

void CDespawnController::detach()
{
	d_object = nullptr;
}

void CDespawnController::update(float dtime)
{
	if (d_object != nullptr && d_active)
	{
		// Update timer
		d_despawnTime -= dtime;
		if (d_despawnTime <= 0.f)
		{
			// Mark for deletion
			d_object->markDeleted();
		}
	}
}

void CDespawnController::setActive(bool state)
{
	d_active = state;
}
