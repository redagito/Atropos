#pragma once

class GameObject; /**< Game object forward declare. */

class IController
{
public:
	virtual ~IController();

	virtual void attach(GameObject* object) = 0;

	virtual void detach() = 0;

	virtual void update(float dtime) = 0;

	virtual void setActive(bool state) = 0;
};
