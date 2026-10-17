#pragma once

#include "game/IController.h"

#include <string>

/**
* \brief Removes object after specified time.
*/
class DespawnController : public IController
{
public:
	/**
	* \brief
	*/
	DespawnController(float despawnTime);

	/**
	* \brief Destructor for cleanup.
	*/
	~DespawnController();

	/**
	* \brief On attach to object.
	*/
	void attach(GameObject* object);

	/**
	* \brief On detaching from game object.
	*/
	void detach();

	/**
	* \brief Updates controller.
	*/
	void update(float dtime);

	void setActive(bool active);

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_despawnTime;
	bool d_active;
};
