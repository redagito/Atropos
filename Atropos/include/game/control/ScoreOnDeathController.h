#pragma once

#include "game/IController.h"

/**
* \brief Adds to a score the first time the controlled object dies. Polls the
* death flag like the other death controllers rather than using an event.
*/
class ScoreOnDeathController : public IController
{
public:
	ScoreOnDeathController(int* score);

	~ScoreOnDeathController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	int* d_score; /**< Counter to increase, owned by the caller. */
	bool d_active; /**< Active flag. */
	bool d_counted; /**< Whether this death was already counted. */
};