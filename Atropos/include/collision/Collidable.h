#pragma once

#include "collision/AABBox.h"

#include "glm/glm.hpp"

/**
* \brief Collision entity that deals damage to the other entity on contact.
*/
class Collidable
{
public:
	/**
	* \brief The entity is created with its group id and the untransformed bounding volume
	*/
	Collidable(unsigned int groupId, const AABBox& box);

	unsigned int getGroupId() const;

	void setDamage(float damage);

	float getDamage() const;

	/**
	* \brief Damage received since the last call; the counter resets on every call.
	*/
	float getDamageReceived();

	void receiveDamage(float damage);

	const AABBox& getAABBox() const;

	void setTranslation(const glm::mat4& translation);

	void setScale(const glm::mat4& scale);

	bool isCollidable() const;

	void markDeleted();

	bool deleteRequested() const;

protected:
	AABBox d_boxTransformed; /**< Transformed collision volume, recalculated on setTranslation call. */
	AABBox d_box;  /**< The untransformed collision volume. */
	unsigned int d_groupId;

	float d_damageReceived; /**< Damage received since the last getDamageReceived call. */
	float d_damageDealt;

	bool d_deleteMe;
	bool d_collidable;
};