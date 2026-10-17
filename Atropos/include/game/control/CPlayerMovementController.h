#pragma once

#include "game/IController.h"

/**
* \brief Player side movement.
*/
class CPlayerMovementController : public IController
{
public:
	CPlayerMovementController(float speedSide);

	/**
	* \brief Destructor for cleanup.
	*/
	~CPlayerMovementController();

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
	* \brief Sets active state.
	*/
	void setActive(bool state);

private:
	CGameObject* d_object; /**< Controlled game object. */
	float d_speedSide; /**< Side movement speed. */
	float d_rotationDegree; /**< Current rotation in degree. */
	bool d_active; /**< Active state flag. */
};
