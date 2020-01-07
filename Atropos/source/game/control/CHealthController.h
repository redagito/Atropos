#pragma once

#include "game/IController.h"

/**
* \brief Stores health state and marks object for deletion on death.
* TODO This should actually send an onDeath event to the object.
*/
class CHealthController : public IController
{
public:
	/**
	* \brief Sets health.
	*/
	CHealthController(float health);

	/**
	* \brief Destructor for cleanup.
	*/
	~CHealthController();

	/**
	* \brief On attach to object.
	*/
	void attach(CGameObject* object);

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
	CGameObject* d_object; /**< Controlled game object. */
	float d_health; /**< Health value. */
	bool d_active; /**< Cative flag. */
};
