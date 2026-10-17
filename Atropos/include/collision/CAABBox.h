#pragma once

#include "glm/glm.hpp"
#include <string>

/**
* \brief Axis aligned bounding box
*/
class CAABBox
{
public:
	/**
	* \brief Returns middle of the box
	*/
	const glm::vec3& getMid() const;

	/**
	* \brief Returns the halfwidths
	*/
	const glm::vec3& getHalfWidths() const;

	/**
	* \brief Sets the mid point
	*/
	void setMid(const glm::vec3& middle);
	
	/**
	* \brief Sets halfwidth vector
	*/
	void setHalfWidths(const glm::vec3& halfWidths);

	/**
	* \brief Returns string representation of the bounding box.
	* Used for debugging.
	*/
	std::string toString() const;

private:
	glm::vec3 d_middlePoint; /**< Middle of the box. */
	glm::vec3 d_halfWidth; /**< Halfwidths of the box. */
};

/**
* \brief Collision check.
*/
bool collides(const CAABBox& b1, const CAABBox& b2);