#pragma once

#include "game/IController.h"

#include <glm/glm.hpp>

/**
* \brief Linear movement controller.
* Updates object position with linear movement.
*/
class CLinearMovementController : public IController
{
public:
	CLinearMovementController(const glm::vec3& velocity);

	/**
	* \brief Destructor for cleanup.
	*/
	~CLinearMovementController();

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
	glm::vec3 d_velocity; /**< Movement vector. */
	bool d_active; /**< Active state flag. */
};
