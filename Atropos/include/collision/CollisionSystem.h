#pragma once

#include "collision/AABBox.h"

#include <vector>
#include <list>

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

private:
	/**
	* \brief Tests the entity for collision with all other entities not in the group
	*/
	void testCollision(Collidable* entity, unsigned int groupId);

	std::vector<std::list<Collidable*>> d_entities; /**< Stores entities by group id (vector index) in lists. */
};
