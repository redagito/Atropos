#pragma once

#include "game/IController.h"

#include <string>

/**
* \brief Removes object after specified time.
*/
class CDespawnController : public IController
{
public:
	/**
	* \brief
	*/
	CDespawnController(float despawnTime);

	/**
	* \brief Destructor for cleanup.
	*/
	~CDespawnController();

	/**
	* \brief On attach to object.
	*/
	void attach(CGameObject* object);

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
	CGameObject* d_object; /**< Controlled game object. */
	float d_despawnTime;
	bool d_active;
};
