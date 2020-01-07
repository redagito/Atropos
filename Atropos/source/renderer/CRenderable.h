#pragma once

class CModel; /**< Model forward declare. */
class CRenderer; /**< Renderer forward declare. */
class CCamera; /**< Camera forward declare. */

#include "IRenderable.h"
#include "collision/CAABBox.h"

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <mutex>

/**
* \brief Drawable entity class for the render system.
*
* A renderable is a graphical object in the scene.
* It has a model (mesh and material) and transformation matrices.
* The object can be named for debugging.
* The transformation matrices are updated with a proxy object.
* The model matrix needs to be calculated separately for hierarchical
* animation.
*/
class CRenderable : public IRenderable
{
public:
	/**
	* \brief Standard constructor, sets name to "unnamed"
	*/
	CRenderable(CModel* model);

	/**
	* \brief Constructor sets name.
	*/
	CRenderable(CModel* model, const char* name);
	
	/**
	* \brief Returns the transformed axis aligned bounding box for the entity.
	*/
	const CAABBox& getAABBox() const;

	/**
	* \brief Returns the translation matrix
	*/
	const glm::mat4& getTranslationMatrix() const;

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
	void draw(const std::shared_ptr<CRenderer>& renderer, const CCamera& camera) const;

	/**
	* \brief Returns the entity name
	*/
	const char* getName() const;

	/**
	* \brief Returns pointer to model resource
	*/
	CModel* getModel() const;

	/**
	* \brief Sets whether the object was culled during the last drawing call or not
	*/
	void setCullingState(bool state);

	/**
	* \brief Sets the objects visibility.
	*/
	void setVisibility(bool state);

	/**
	* \brief Returns visibility setting of the object.
	*/
	bool getVisibility() const;

	/**
	* \brief Returns the culling state of the entity
	*/
	bool isCulled() const;
	bool isVisible() const;

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

	/**
	* \brief Returns whether the model creates shadows.
	* TODO This should be in the material.
	*/
	bool hasShadow() const;

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

	std::string d_name; /**< Name for debugging. */
	CAABBox d_box; /**< Transformed axis-aligned bounding box. */

	CModel* d_modelPtr; /**< Pointer to shared model resource. */

	glm::mat4 d_model; /**< Model matrix. */
	glm::mat4 d_rotation; /**< Rotation matrix. */
	glm::mat4 d_translation; /**< Translation matrix. */
	glm::mat4 d_scale; /**< Scale matrix. */

	bool d_hasShadow; /**< Object creates shadow. */
	bool d_deleteMe; /**< Delete request flag. */
	bool d_isCulled; /**< Culling state. */
	bool d_visible; /**< Visibility state. */
};
