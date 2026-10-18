#pragma once

class Model;
class Renderer;
class Camera;

#include "collision/AABBox.h"

#include "glm/glm.hpp"

#include <memory>

/**
 * \brief Drawable entity: a model (mesh and material) plus transform matrices.
 *
 * The model matrix must be calculated separately for hierarchical animation.
 */
class Renderable
{
public:
	Renderable(Model* model);
	
	/**
	* \brief Returns the transformed axis aligned bounding box for the entity.
	*/
	const AABBox& getAABBox() const;

	void setModelMatrix(const glm::mat4& matrix);
	
	void setRotationMatrix(const glm::mat4& matrix);
	
	void setTranslationMatrix(const glm::mat4& matrix);

	void setScaleMatrix(const glm::mat4& matrix);

	void draw(const std::shared_ptr<Renderer>& renderer, const Camera& camera) const;

	Model* getModel() const;

	void markDeleted();

	bool deleteRequested() const;

	/**
	* \brief Sets whether the object creates a shadow.
	* TODO This should be in the material.
	*/
	void setShadow(bool);

protected:
	/**
	* \brief Updates the bounding box position.
	* Call after changing the translation matrix.
	*/
	void updateAABBoxPosition();

	/**
	* \brief Updates the bounding box size.
	* Call after changing the scale matrix.
	*/
	void updateAABBoxSize();

	AABBox d_box; /**< Transformed axis-aligned bounding box. */

	Model* d_modelPtr; /**< Pointer to shared model resource. */

	glm::mat4 d_model;
	glm::mat4 d_rotation;
	glm::mat4 d_translation;
	glm::mat4 d_scale;

	bool d_hasShadow;
	bool d_deleteMe;
};
