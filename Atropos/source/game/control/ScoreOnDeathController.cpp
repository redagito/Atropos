#include "game/control/ScoreOnDeathController.h"

#include "game/GameObject.h"

ScoreOnDeathController::ScoreOnDeathController(int* score)
	:
	d_object(nullptr),
	d_score(score),
	d_active(true),
	d_counted(false)
{
	return;
}

ScoreOnDeathController::~ScoreOnDeathController()
{
	return;
}

void ScoreOnDeathController::attach(GameObject* object)
{
	d_object = object;
}

void ScoreOnDeathController::detach()
{
	d_object = nullptr;
}

void ScoreOnDeathController::update(float)
{
	if (d_active && d_score != nullptr && d_object != nullptr && d_object->isDead() && !d_counted)
	{
		++(*d_score);
		d_counted = true;
	}
}

void ScoreOnDeathController::setActive(bool active)
{
	d_active = active;
}