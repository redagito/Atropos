#pragma once

#include "game/IController.h"

// TODO: Should send an onDeath event to the object instead of setting the dead flag.
class HealthController : public IController
{
public:
	HealthController(float health);

	~HealthController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool state);

	float getHealth() const;

	float getMaxHealth() const;

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_health; /**< Health value. */
	float d_maxHealth; /**< Health the object started with. */
	bool d_active; /**< Active flag. */
};
