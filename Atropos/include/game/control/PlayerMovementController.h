#pragma once

#include "game/IController.h"

/**
* \brief Player side movement.
*/
class PlayerMovementController : public IController
{
public:
	PlayerMovementController(float speedSide);

	/**
	* \brief Destructor for cleanup.
	*/
	~PlayerMovementController();

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
	* \brief Sets active state.
	*/
	void setActive(bool state);

private:
	GameObject* d_object; /**< Controlled game object. */
	float d_speedSide; /**< Side movement speed. */
	float d_rotationDegree; /**< Current rotation in degree. */
	bool d_active; /**< Active state flag. */
};
