#include "game/GameWorld.h"

#include <cassert>



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
	assert(object != nullptr);
	d_objects.push_back(std::unique_ptr<GameObject>(object));
}
