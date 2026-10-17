#pragma once

class Camera;

#include "game/IController.h"

/**
* \brief Controls camera movement based on the attached game object.
*/
class CameraController : public IController
{
public:
	/**
	* \brief Sets camera.
	*/
	CameraController(Camera* camera);

	/**
	* \brief Destructor for cleanup.
	*/
	~CameraController();

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

	/**
	* \brief Sets active state.
	*/
	void setActive(bool state);

private:
	Camera* d_camera; /**< Controlled camera. */
	GameObject* d_object; /**< Controlled game object. */
	bool d_active; /**< Active state flag. */
};
