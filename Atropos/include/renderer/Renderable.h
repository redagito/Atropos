#pragma once

class Model; /**< Model forward declare. */
class Renderer; /**< Renderer forward declare. */
class Camera; /**< Camera forward declare. */

#include "collision/AABBox.h"

#include "glm/glm.hpp"

#include <memory>

/**
* \brief Drawable entity class for the render system.
*
* A renderable is a graphical object in the scene.
* It has a model (mesh and material) and transformation matrices.
* The transformation matrices are updated with a proxy object.
* The model matrix needs to be calculated separately for hierarchical
* animation.
*/
class Renderable
{
public:
	/**
	* \brief Standard constructor
	*/
	Renderable(Model* model);
	
	/**
	* \brief Returns the transformed axis aligned bounding box for the entity.
	*/
	const AABBox& getAABBox() const;

	/**
	* \brief Sets model matrix
	*/
	void setModelMatrix(const glm::mat4& matrix);
	
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
	* \brief Draws self with renderer.
	*/
	void draw(const std::shared_ptr<Renderer>& renderer, const Camera& camera) const;

	/**
	* \brief Returns pointer to model resource
	*/
	Model* getModel() const;

	/**
	* \brief Requests deletion of the object
	*/
	void markDeleted();

	/**
	* \brief Returns whether the object can be deleted safely
	*/
	bool deleteRequested() const;

	/**
	* \brief Sets whether the object creates a shadow.
	* TODO This should be in the material.
	*/
	void setShadow(bool);

protected:
	/**
	* \brief Updates the bounding box position.
	*
	* Transforms the AABBox with the translation matrix. Call after changes to
	* the translation matrix.
	*/
	void updateAABBoxPosition();

	/**
	* \brief Updates the bounding box size.
	*
	* Transforms the halfwidth vector of the bounding box by the scale matrix.
	* Call after changes to the scale matrix.
	*/
	void updateAABBoxSize();

	AABBox d_box; /**< Transformed axis-aligned bounding box. */

	Model* d_modelPtr; /**< Pointer to shared model resource. */

	glm::mat4 d_model; /**< Model matrix. */
	glm::mat4 d_rotation; /**< Rotation matrix. */
	glm::mat4 d_translation; /**< Translation matrix. */
	glm::mat4 d_scale; /**< Scale matrix. */

	bool d_hasShadow; /**< Object creates shadow. */
	bool d_deleteMe; /**< Delete request flag. */
};
