#include "collision/CollisionSystem.h"

#include "collision/Collidable.h"
#include <spdlog/spdlog.h>

#include <stdexcept>

unsigned int CollisionSystem::getNewGroupId()
{
	// Add new collision group
	d_entities.push_back(std::list<Collidable*>());
	// Return the group id
	return static_cast<unsigned int>(d_entities.size() - 1);
}

// Cleanup
CollisionSystem::~CollisionSystem()
{
	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		for (auto iter = d_entities[i].begin(); iter != d_entities[i].end(); ++iter)
		{
			delete (*iter);
		}
	}
}

Collidable* CollisionSystem::add(const AABBox& box, unsigned int groupId)
{
	if (groupId >= d_entities.size())
	{
		SPDLOG_ERROR("Group id {} does not exist", groupId);
		throw std::runtime_error("CollisionSystem.add: Invalid goup id");
	}
	// Create new entity and add to the collision group
	Collidable* entity = new Collidable(groupId, box);
	d_entities[groupId].push_back(entity);
	return entity;
}

// Removes collidable entity
void CollisionSystem::remove(Collidable* entity)
{
	if (entity == nullptr)
	{
		return;
	}
	// Defer the deletion to update() so the entity is never freed while it may
	// still be referenced, and removing twice is harmless.
	entity->markDeleted();
}

// Test all entities for collision
void CollisionSystem::update()
{
	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		auto iter = d_entities[i].begin(); 
		while (iter != d_entities[i].end())
		{
			// Delete if requested
			if ((*iter)->deleteRequested())
			{
				Collidable* temp = (*iter);
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
void CollisionSystem::testCollision(Collidable* entity, unsigned int group)
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
					Collidable* temp = (*iter);
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