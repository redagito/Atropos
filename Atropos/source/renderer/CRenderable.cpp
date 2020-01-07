#include "CRenderable.h"
#include "resource/CModel.h"
#include "render/CRenderer.h"

// Sets name to unnamed
CRenderable::CRenderable(CModel* model)
	:
	d_name("unnamed"),
	d_modelPtr(model),
	d_deleteMe(false),
	d_rotation(glm::mat4(1.f)),
	d_translation(glm::mat4(1.f)),
	d_scale(glm::mat4(1.f)),
	d_hasShadow(true),
	d_isCulled(true),
	d_visible(true)
{
	d_box = model->getMesh()->getAABBox();
}

// Sets name
CRenderable::CRenderable(CModel* model, const char* name)
	:
	d_name(name),
	d_modelPtr(model),
	d_deleteMe(false),
	d_rotation(glm::mat4(1.f)),
	d_translation(glm::mat4(1.f)),
	d_scale(glm::mat4(1.f)),
	d_hasShadow(true),
	d_isCulled(true),
	d_visible(true)
{
	d_box = model->getMesh()->getAABBox();
}

// Read access to the translation matrix, needed for culling
const glm::mat4& CRenderable::getTranslationMatrix() const
{
	return d_translation;
}

// Set model matrix
void CRenderable::setModelMatrix(const glm::mat4& matrix)
{
	d_model = matrix;
}

// Set rotation matrix
void CRenderable::setRotationMatrix(const glm::mat4& matrix)
{
	d_rotation = matrix;
}

// Set translation matrix
void CRenderable::setTranslationMatrix(const glm::mat4& matrix)
{
	d_translation = matrix;
	updateAABBoxPosition();
}

void CRenderable::updateAABBoxPosition()
{
	// Translate box centre
	d_box.setMid(glm::vec3(d_translation * glm::vec4(d_modelPtr->getMesh()->getAABBox().getMid(), 1.f)));
}

void CRenderable::updateAABBoxSize()
{
	// Scale box halfwidths
	d_box.setHalfWidths(glm::vec3(d_scale * glm::vec4(d_modelPtr->getMesh()->getAABBox().getHalfWidths(), 1.f)));
}

// Set scale matrix
void CRenderable::setScaleMatrix(const glm::mat4& matrix)
{
	d_scale = matrix;
	updateAABBoxSize();
}

// Draw with renderer
void CRenderable::draw(const std::shared_ptr<CRenderer>& renderer, const CCamera& camera) const
{
	renderer->draw(	getModel()->getVertexArrayObject(), 
					getModel()->getMesh()->getVertexBuffer()->getSize() / 3, 
					getModel()->getMaterial(), 
					d_model,
					d_rotation,
					d_translation,
					d_scale,
					d_hasShadow);
}

// Returns entity name
const char* CRenderable::getName() const
{
	return d_name.c_str();
}

// Returns model
CModel* CRenderable::getModel() const
{
	return d_modelPtr;
}

// Returns transformed bounding volume
const CAABBox& CRenderable::getAABBox() const
{
	return d_box;
}

// Returns visibility of the object
bool CRenderable::isCulled() const
{
	return d_isCulled;
}

bool CRenderable::isVisible() const
{
	return !d_isCulled && d_visible;
}

// Set to true if culled -> not visible, or false -> visible
void CRenderable::setCullingState(bool state)
{
	d_isCulled = state;
}

void CRenderable::setVisibility(bool state)
{
	d_visible = state;
}

bool CRenderable::getVisibility() const
{
	return d_visible;
}

bool CRenderable::deleteRequested() const
{
	return d_deleteMe;
}

void CRenderable::markDeleted()
{
	d_deleteMe = true;
}

void CRenderable::setShadow(bool hasShadow)
{
	d_hasShadow = hasShadow;
}

bool CRenderable::hasShadow() const
{
	return d_hasShadow;
}
