#pragma once

class CRenderable;

#include "IRenderable.h"

/**
 * \brief Proxy class for renderable objects.
 */
class CRenderableProxy : public IRenderable
{
public:
	CRenderableProxy(CRenderable* renderable);
	~CRenderableProxy();

	/**
	* \brief Sets rotation matrix
	*/
	void setRotationMatrix(const glm::mat4& matrix);

	/**
	* \brief Sets translation matrix
	*/
	void setTranslationMatrix(const glm::mat4& matrix);

	/**
	* \brief Sets scale matrix
	*/
	void setScaleMatrix(const glm::mat4& matrix);

	/**
	* \brief Returns current visibility state of the object.
	* \return True if the object is visible (not culled) and false otherwise.
	*/
	bool isVisible() const;

private:
	CRenderable* d_renderable; /**< Renderable object. */
};
