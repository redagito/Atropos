#include "renderer/CRenderable.h"
#include "renderer/resource/CModel.h"
#include "renderer/render/CRenderer.h"

#include "debug/Log.h"

// Sets name to unnamed
CRenderable::CRenderable(CModel* model)
	:
	d_name("unnamed"),
	d_modelPtr(model),
	d_deleteMe(false),
	d_model(glm::mat4(1.f)),
	d_rotation(glm::mat4(1.f)),
	d_translation(glm::mat4(1.f)),
	d_scale(glm::mat4(1.f)),
	d_hasShadow(true),
	d_isCulled(true),
	d_visible(true)
{
	if (model != nullptr && model->getMesh() != nullptr)
	{
		d_box = model->getMesh()->getAABBox();
	}
	else
	{
		LOG_ERROR("Failed to create renderable %s: missing model or mesh.", d_name.c_str());
	}
}

// Sets name
CRenderable::CRenderable(CModel* model, const char* name)
	:
	d_name(name),
	d_modelPtr(model),
	d_deleteMe(false),
	d_model(glm::mat4(1.f)),
	d_rotation(glm::mat4(1.f)),
	d_translation(glm::mat4(1.f)),
	d_scale(glm::mat4(1.f)),
	d_hasShadow(true),
	d_isCulled(true),
	d_visible(true)
{
	if (model != nullptr && model->getMesh() != nullptr)
	{
		d_box = model->getMesh()->getAABBox();
	}
	else
	{
		LOG_ERROR("Failed to create renderable %s: missing model or mesh.", d_name.c_str());
	}
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

// Set to true if culled -> not visible, or false -> visible
void CRenderable::setCullingState(bool state)
{
	d_isCulled = state;
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
