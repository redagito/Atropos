#pragma once

#include "collision/AABBox.h"

#include <vector>
#include <list>
#include <set>
#include <utility>

class Collidable;

class CollisionSystem
{
public:
	CollisionSystem() = default;

	~CollisionSystem();

	CollisionSystem(const CollisionSystem&) = delete;
	CollisionSystem& operator=(const CollisionSystem&) = delete;

	Collidable* add(const AABBox& box, unsigned int group);

	/**
	* \brief Marks a collidable for removal; it is freed on the next update().
	*/
	void remove(Collidable* entity);

	void update();

	unsigned int getNewGroupId();

	/**
	* \brief Releases a group id for reuse; every collidable still in it is deleted, so the caller must drop its references.
	*/
	void releaseGroup(unsigned int groupId);

private:
	/**
	* \brief Narrowphase test of the entity against every collidable outside its group.
	*/
	void testCollision(Collidable* entity, unsigned int groupId);

	/**
	* \brief Drops every open contact of an entity that is about to be freed.
	*/
	void forgetContacts(Collidable* entity);

	std::vector<std::list<Collidable*>> d_entities; /**< Stores entities by group id (vector index) in lists. */
	std::vector<bool> d_groupFree; /**< Whether a group id has been released and may be reused. */
	std::vector<unsigned int> d_freeGroups;

	// Ordered as (dealer, receiver), so both directions of a contact are tracked separately.
	using Contact = std::pair<Collidable*, Collidable*>;
	std::set<Contact> d_openContacts; /**< Contacts that were already touching during the last update. */
	std::set<Contact> d_currentContacts;
};
