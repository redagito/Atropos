#include "game/GameWorld.h"

#include <stdexcept>



void GameWorld::update(float dtime)
{
	for (auto iter = d_objects.begin(); iter != d_objects.end();)
	{
		(*iter)->update(dtime);
		if ((*iter)->isDeleteRequested())
		{
			iter = d_objects.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void GameWorld::addObject(GameObject* object)
{
	// Not an assert: Release builds compile it away and would store a null unique_ptr.
	if (object == nullptr)
	{
		throw std::runtime_error("Cannot add a null game object.");
	}
	d_objects.push_back(std::unique_ptr<GameObject>(object));
}

void GameWorld::clear()
{
	d_objects.clear();
}
