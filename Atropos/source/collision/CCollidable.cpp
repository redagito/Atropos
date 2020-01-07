#include "CCollidable.h"

// Custom memory manager


CCollidable::CCollidable(unsigned int group, const CAABBox& box)
	:	
	d_groupId(group),
	d_damageDealt(0.f),
	d_damageReceived(0.f),
	d_box(box),
	d_boxTransformed(box),
	d_deleteMe(false),
	d_collidable(true)
{
	return;
}

float CCollidable::getDamage() const
{
	return d_damageDealt;
}

// Returns accumuated damage and resets damage counter
float CCollidable::getDamageReceived()
{
	float temp = d_damageReceived;
	d_damageReceived = 0.f;
	return temp;
}

// Returns group id
unsigned int CCollidable::getGroupId() const
{
	return d_groupId;
}

// Add damage to the internal counter
void CCollidable::receiveDamage(float damage)
{
	d_damageReceived += damage;
}

// Sets the damage dealt by this entity
void CCollidable::setDamage(float damage)
{
	d_damageDealt = damage;
}

const CAABBox& CCollidable::getAABBox() const
{
	return d_boxTransformed;
}

void CCollidable::setScale(const glm::mat4& scale)
{
	d_boxTransformed.setHalfWidths(glm::vec3(scale * glm::vec4(d_box.getHalfWidths(), 1.f)));
	return;
}

void CCollidable::setTranslation(const glm::mat4& translate)
{
	d_boxTransformed.setMid(glm::vec3(translate * glm::vec4(d_box.getMid(), 1.f)));
	return;
}

// Sets collidable state
void CCollidable::setCollidable(bool state)
{
	d_collidable = state;
}

// Returns collidable state
bool CCollidable::isCollidable() const
{
	return d_collidable;
}

bool CCollidable::deleteRequested() const
{
	return d_deleteMe;
}

void CCollidable::markDeleted()
{
	d_deleteMe = true;
}