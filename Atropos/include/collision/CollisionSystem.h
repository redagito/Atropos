#pragma once

#include "collision/AABBox.h"

#include <vector>
#include <list>
#include <set>
#include <utility>

// Forward declare
class Collidable;

class CollisionSystem
{
public:
	CollisionSystem() = default;

	/**
	* \brief Cleanup of resources.
	*/
	~CollisionSystem();

	CollisionSystem(const CollisionSystem&) = delete;
	CollisionSystem& operator=(const CollisionSystem&) = delete;

	/**
	* \brief Creates a new collidable and adds it to the specified group.
	*
	* A collidable entity has a bounding box and a transformation matrix.
	* Collidables in the same group are not tested for collision.
	* \param The untransformed bounding box for the entity
	*/
	Collidable* add(const AABBox& box, unsigned int group);

	/**
	* \brief Marks a collidable entity for removal.
	*
	* The entity is freed on the next call to update().
	*/
	void remove(Collidable* entity);

	/**
	* \brief Tests the entities for collision
	*/
	void update();

	/**
	* \brief Creates and returns new collision group id
	*/
	unsigned int getNewGroupId();

	/**
	* \brief Frees a collision group id so it can be reused.
	*
	* All collidables still in the group are deleted. The caller must ensure
	* that no references to those collidables remain.
	*/
	void releaseGroup(unsigned int groupId);

private:
	/**
	* \brief Tests the entity for collision with all other entities not in the group
	*/
	void testCollision(Collidable* entity, unsigned int groupId);

	/**
	* \brief Drops every open contact of an entity that is about to be freed.
	*/
	void forgetContacts(Collidable* entity);

	std::vector<std::list<Collidable*>> d_entities; /**< Stores entities by group id (vector index) in lists. */
	std::vector<bool> d_groupFree; /**< Whether a group id has been released and may be reused. */
	std::vector<unsigned int> d_freeGroups; /**< Released group ids available for reuse. */

	// Damage is dealt once when two entities start touching, not on every frame
	// they stay overlapping. The pair is ordered as dealer then receiver, so the
	// two directions of a contact are tracked separately.
	using Contact = std::pair<Collidable*, Collidable*>;
	std::set<Contact> d_openContacts; /**< Contacts that were already touching during the last update. */
	std::set<Contact> d_currentContacts; /**< Contacts found during the running update. */
};
