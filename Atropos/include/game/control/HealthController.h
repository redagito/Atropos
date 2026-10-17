#pragma once

#include "game/IController.h"

/**
* \brief Stores health state and sets the object into the death state on death.
* TODO This should actually send an onDeath event to the object.
*/
class HealthController : public IController
{
public:
	/**
	* \brief Sets health.
	*/
	HealthController(float health);

	/**
	* \brief Destructor for cleanup.
	*/
	~HealthController();

	/**
	* \brief On attach to object.
	*/
	void attach(GameObject* object);

	/**
	* \brief On dettaching from game object.
	*/
	void detach();

	/**
	* \brief Updates controller.
	*/
	void update(float dtime);

	/**
	* \brief Active state.
	*/
	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_health; /**< Health value. */
	bool d_active; /**< Active flag. */
};
