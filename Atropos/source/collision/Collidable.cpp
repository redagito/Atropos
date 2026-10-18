#include "collision/Collidable.h"



Collidable::Collidable(unsigned int group, const AABBox& box)
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

float Collidable::getDamage() const
{
	return d_damageDealt;
}

float Collidable::getDamageReceived()
{
	float temp = d_damageReceived;
	d_damageReceived = 0.f;
	return temp;
}

unsigned int Collidable::getGroupId() const
{
	return d_groupId;
}

void Collidable::receiveDamage(float damage)
{
	d_damageReceived += damage;
}

void Collidable::setDamage(float damage)
{
	d_damageDealt = damage;
}

const AABBox& Collidable::getAABBox() const
{
	return d_boxTransformed;
}

void Collidable::setScale(const glm::mat4& scale)
{
	d_boxTransformed.setHalfWidths(glm::vec3(scale * glm::vec4(d_box.getHalfWidths(), 1.f)));
	return;
}

void Collidable::setTranslation(const glm::mat4& translate)
{
	d_boxTransformed.setMid(glm::vec3(translate * glm::vec4(d_box.getMid(), 1.f)));
	return;
}

bool Collidable::isCollidable() const
{
	return d_collidable;
}

bool Collidable::deleteRequested() const
{
	return d_deleteMe;
}

void Collidable::markDeleted()
{
	d_deleteMe = true;
}
