#include "CGameWorld.h"

#include <cassert>



void CGameWorld::update(float dtime)
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

void CGameWorld::addObject(CGameObject* object)
{
	assert(object != nullptr);
	d_objects.push_back(std::unique_ptr<CGameObject>(object));
}
