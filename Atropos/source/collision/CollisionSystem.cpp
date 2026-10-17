#include "collision/CollisionSystem.h"

#include "collision/Collidable.h"

#include <stdexcept>
#include <string>

unsigned int CollisionSystem::getNewGroupId()
{
	// Reuse a released group id if available
	if (!d_freeGroups.empty())
	{
		unsigned int id = d_freeGroups.back();
		d_freeGroups.pop_back();
		d_groupFree[id] = false;
		return id;
	}
	// Add new collision group
	d_entities.push_back(std::list<Collidable*>());
	d_groupFree.push_back(false);
	// Return the group id
	return static_cast<unsigned int>(d_entities.size() - 1);
}

void CollisionSystem::releaseGroup(unsigned int groupId)
{
	if (groupId >= d_entities.size() || d_groupFree[groupId])
	{
		return;
	}
	// Delete remaining entities and hand the id back for reuse
	for (Collidable* entity : d_entities[groupId])
	{
		forgetContacts(entity);
		delete entity;
	}
	d_entities[groupId].clear();
	d_groupFree[groupId] = true;
	d_freeGroups.push_back(groupId);
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
		throw std::runtime_error("CollisionSystem.add: Invalid group id " + std::to_string(groupId));
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

// Drop the open contacts of an entity that is about to be freed. The address can
// be handed to a new collidable, which must not inherit the contacts of the old
// one and silently skip its damage.
void CollisionSystem::forgetContacts(Collidable* entity)
{
	for (auto contact = d_openContacts.begin(); contact != d_openContacts.end();)
	{
		if (contact->first == entity || contact->second == entity)
		{
			contact = d_openContacts.erase(contact);
		}
		else
		{
			++contact;
		}
	}
}

// Test all entities for collision
void CollisionSystem::update()
{
	d_currentContacts.clear();

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
				forgetContacts(temp);
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

	// Whatever is not touching any more closes its contact, so a later overlap
	// counts as a new one.
	d_openContacts.swap(d_currentContacts);
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
					forgetContacts(temp);
					delete temp;
				}
				else
				{
					if ((*iter)->isCollidable())
					{
if (collides(entity->getAABBox(), (*iter)->getAABBox()))
					{
						// Since every collision is tested both ways, damage only happens from first entity to second entity
						const Contact contact(entity, *iter);
						// A contact deals its damage once, when it opens. Repeating
						// it every frame while the two overlap would make the damage
						// depend on the frame rate instead of on the collision.
						if (d_openContacts.count(contact) == 0)
						{
							(*iter)->receiveDamage(entity->getDamage());
						}
						d_currentContacts.insert(contact);
					}
					}
					++iter;
				}
			}
		}
	}
}