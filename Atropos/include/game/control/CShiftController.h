#pragma once

class CCamera;

#include "game/IController.h"

#include <glm/glm.hpp>

/**
* \brief Shift movement controller based on camera.
*/
class CShiftController : public IController
{
public:
	CShiftController(const CCamera* camera, float shiftDistance);

	/**
	* \brief Destructor for cleanup.
	*/
	~CShiftController();

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
	float d_shiftDistance; /**< Shifting distance. */
	const CCamera* d_camera; /**< Active camera object. */
	bool d_active; /**< Active state flag. */
};
