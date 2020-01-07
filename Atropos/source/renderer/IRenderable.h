#pragma once

#include <glm/glm.hpp>

/**
 * \brief Renderable interface class.
 */
class IRenderable
{
public:
	/**
	 * \brief Virtual
	 */
	virtual ~IRenderable();

	/**
	* \brief Sets rotation matrix
	*/
	virtual void setRotationMatrix(const glm::mat4& matrix) = 0;

	/**
	* \brief Sets translation matrix
	*/
	virtual void setTranslationMatrix(const glm::mat4& matrix) = 0;

	/**
	* \brief Sets scale matrix
	*/
	virtual void setScaleMatrix(const glm::mat4& matrix) = 0;

	/**
	* \brief Returns current visibility state of the object.
	* \return True if the object is visible (not culled) and false otherwise.
	*/
	virtual bool isVisible() const = 0;
};
