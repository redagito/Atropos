#pragma once

class Camera;

#include "game/IController.h"

class CameraController : public IController
{
public:
	CameraController(Camera* camera);

	~CameraController();

	void attach(GameObject* object);

	void detach();

	void update(float dtime);

	void setActive(bool state);

private:
	Camera* d_camera; /**< Controlled camera. */
	GameObject* d_object; /**< Controlled game object. */
	bool d_active; /**< Active state flag. */
};
