#pragma once

class Camera;

#include "game/IController.h"

#include <glm/glm.hpp>

/**
* \brief Shift movement controller based on camera.
*/
class ShiftController : public IController
{
public:
	ShiftController(const Camera* camera, float shiftDistance);

	/**
	* \brief Destructor for cleanup.
	*/
	~ShiftController();

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
	float d_shiftDistance; /**< Shifting distance. */
	const Camera* d_camera; /**< Active camera object. */
	bool d_active; /**< Active state flag. */
};
