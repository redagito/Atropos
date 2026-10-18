#pragma once

#include "glm/glm.hpp"
#include <string>

class AABBox
{
public:
	/**
	* \brief Creates an empty box at the origin.
	*/
	AABBox();

	const glm::vec3& getMid() const;

	const glm::vec3& getHalfWidths() const;

	void setMid(const glm::vec3& middle);
	
	void setHalfWidths(const glm::vec3& halfWidths);

private:
	glm::vec3 d_middlePoint;
	glm::vec3 d_halfWidth;
};

bool collides(const AABBox& b1, const AABBox& b2);