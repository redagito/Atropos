#pragma once

class CCamera;

#include "game/IController.h"

/**
* \brief Controls camera movement based on the attached game object.
*/
class CCameraController : public IController
{
public:
	/**
	* \brief Sets camera.
	*/
	CCameraController(CCamera* camera);

	/**
	* \brief Destructor for cleanup.
	*/
	~CCameraController();

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

	/**
	* \brief Sets active state.
	*/
	void setActive(bool state);

private:
	CCamera* d_camera; /**< Controlled camera. */
	CGameObject* d_object; /**< Controlled game object. */
	bool d_active; /**< Active state flag. */
};
