#pragma once

#include "game/GameObject.h"

#include <list>
#include <memory>

class GameWorld
{
public:
	void update(float dtime);

	void addObject(GameObject* object);

	void clear();

private:
	std::list<std::unique_ptr<GameObject>> d_objects; /**< Game objects. */
};
