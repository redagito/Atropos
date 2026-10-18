#include "collision/CollisionSystem.h"

#include "collision/Collidable.h"

#include <stdexcept>
#include <string>

unsigned int CollisionSystem::getNewGroupId()
{
	if (!d_freeGroups.empty())
	{
		unsigned int id = d_freeGroups.back();
		d_freeGroups.pop_back();
		d_groupFree[id] = false;
		return id;
	}
	d_entities.push_back(std::list<Collidable*>());
	d_groupFree.push_back(false);
	return static_cast<unsigned int>(d_entities.size() - 1);
}

void CollisionSystem::releaseGroup(unsigned int groupId)
{
	if (groupId >= d_entities.size() || d_groupFree[groupId])
	{
		return;
	}
	for (Collidable* entity : d_entities[groupId])
	{
		forgetContacts(entity);
		delete entity;
	}
	d_entities[groupId].clear();
	d_groupFree[groupId] = true;
	d_freeGroups.push_back(groupId);
}

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
	Collidable* entity = new Collidable(groupId, box);
	d_entities[groupId].push_back(entity);
	return entity;
}

void CollisionSystem::remove(Collidable* entity)
{
	if (entity == nullptr)
	{
		return;
	}
	// Deferred to update(): the entity may still be referenced when remove() returns.
	entity->markDeleted();
}

// Drop open contacts: the address may be reused, and the new owner must not inherit them.
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

void CollisionSystem::update()
{
	d_currentContacts.clear();

	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		auto iter = d_entities[i].begin(); 
		while (iter != d_entities[i].end())
		{
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

	// Contacts that stopped touching are closed, so a later overlap counts as new.
	d_openContacts.swap(d_currentContacts);
}

void CollisionSystem::testCollision(Collidable* entity, unsigned int group)
{
	for (unsigned int i = 0; i < d_entities.size(); ++i)
	{
		if (i != group)
		{
			auto iter = d_entities[i].begin(); 
			while (iter != d_entities[i].end())
			{
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
						// Tested both ways, so damage flows only from the first entity to the second.
						const Contact contact(entity, *iter);
						// Damage once when the contact opens, so it does not scale with frame rate.
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