#include "collision/AABBox.h"

#include <sstream>
#include <cmath>



AABBox::AABBox()
	:
	d_middlePoint(0.f),
	d_halfWidth(0.f)
{
	return;
}

const glm::vec3& AABBox::getMid() const
{
	return d_middlePoint;
}

const glm::vec3& AABBox::getHalfWidths() const
{
	return d_halfWidth;
}

void AABBox::setMid(const glm::vec3& mid)
{
	d_middlePoint = mid;
}

void AABBox::setHalfWidths(const glm::vec3& halfWidth)
{
	d_halfWidth = halfWidth;
}

bool collides(const AABBox& box1, const AABBox& box2)
{
	glm::vec3 diff = box1.getMid() - box2.getMid();
	glm::vec3 widths = box1.getHalfWidths() + box2.getHalfWidths();
	if (std::abs(diff.x) > widths.x || std::abs(diff.y) > widths.y || std::abs(diff.z) > widths.z)
	{
		return false;
	}
	return true;
}
