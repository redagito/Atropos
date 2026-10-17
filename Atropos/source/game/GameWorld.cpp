#include "game/GameWorld.h"

#include <stdexcept>



void GameWorld::update(float dtime)
{
	for (auto iter = d_objects.begin(); iter != d_objects.end();)
	{
		// Update single game object
		(*iter)->update(dtime);
		if ((*iter)->isDeleteRequested())
		{
			// Delete object
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
	// A real check rather than an assert, because Release builds compile the
	// assert away and would take ownership of nothing.
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
