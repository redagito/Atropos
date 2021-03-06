#include "CCollisionSystem.h"

#include "CCollidable.h"
#include "debug/Log.h"

#include <stdexcept>

unsigned int CCollisionSystem::getNewGroupId()
{
	// Add new collision group
	d_entities.push_back(std::list<CCollidable*>());
	// Return the group id
	return d_entities.size() - 1;
}

// Cleanup
CCollisionSystem::~CCollisionSystem()
{
	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		for (auto iter = d_entities[i].begin(); iter != d_entities[i].end(); ++iter)
		{
			delete (*iter);
		}
	}
}

CCollidable* CCollisionSystem::add(const CAABBox& box, unsigned int groupId)
{
	if (groupId >= d_entities.size())
	{
		LOG_ERROR("Group id %i does not exist", groupId);
		throw std::runtime_error("CCollisionSystem.add: Invalid goup id");
	}
	// Create new entity and add to the collision group
	CCollidable* entity = new CCollidable(groupId, box);
	d_entities[groupId].push_back(entity);
	return entity;
}

// Removes collidable entity
void CCollisionSystem::remove(CCollidable* entity)
{
	d_entities[entity->getGroupId()].remove(entity);
	delete entity;
}

// Test all entities for collision
void CCollisionSystem::update()
{
	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		auto iter = d_entities[i].begin(); 
		while (iter != d_entities[i].end())
		{
			// Delete if requested
			if ((*iter)->deleteRequested())
			{
				CCollidable* temp = (*iter);
				iter = d_entities[i].erase(iter);
				delete temp;
			}
			else
			{
				if ((*iter)->isCollidable())
				{
					testCollision((*iter), i);
				}
				++iter;
			}
		}
	}
}

// Tests collidable entity against all entities not in the group
void CCollisionSystem::testCollision(CCollidable* entity, unsigned int group)
{
	// Everything gets tested twice, make this faster
	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		if (i != group)
		{
			auto iter = d_entities[i].begin(); 
			while (iter != d_entities[i].end())
			{
				// Delete if requested
				if ((*iter)->deleteRequested())
				{
					CCollidable* temp = (*iter);
					iter = d_entities[i].erase(iter);
					delete temp;
				}
				else
				{
					if ((*iter)->isCollidable())
					{
						if (collides(entity->getAABBox(), (*iter)->getAABBox()))
						{
							// Since every collision is tested both ways, damage only happens from first entity to second entity
							(*iter)->receiveDamage(entity->getDamage());
						}
					}
					++iter;
				}
			}
		}
	}
}