#include "CAABBox.h"

#include <sstream>
#include <cmath>

// Custom memory manager


// Returns mid point
const glm::vec3& CAABBox::getMid() const
{
	return d_middlePoint;
}

// Returns halfwidth vector
const glm::vec3& CAABBox::getHalfWidths() const
{
	return d_halfWidth;
}

// Sets middle
void CAABBox::setMid(const glm::vec3& mid)
{
	d_middlePoint = mid;
}

// Sets half width
void CAABBox::setHalfWidths(const glm::vec3& halfWidth)
{
	d_halfWidth = halfWidth;
}

// Returns as string
std::string CAABBox::toString() const
{
	std::stringstream ss("CAABBox:");
	ss << " mid.x =" << d_middlePoint.x;
	ss << " mid.y =" << d_middlePoint.y;
	ss << " mid.z =" << d_middlePoint.z;
	ss << " halfwidth.x =" << d_halfWidth.x;
	ss << " halfwidth.y =" << d_halfWidth.y;
	ss << " halfwidth.z =" << d_halfWidth.z;
	return ss.str(); 
}

bool collides(const CAABBox& box1, const CAABBox& box2)
{
	glm::vec3 diff = box1.getMid() - box2.getMid();
	glm::vec3 widths = box1.getHalfWidths() + box2.getHalfWidths();
	if (std::abs(diff.x) > widths.x || std::abs(diff.y) > widths.y || std::abs(diff.z) > widths.z)
	{
		return false;
	}
	return true;
}