#include "renderer/Renderable.h"
#include "renderer/resource/Model.h"
#include "renderer/render/Renderer.h"

#include <spdlog/spdlog.h>

Renderable::Renderable(Model* model)
	:
	d_modelPtr(model),
	d_model(glm::mat4(1.f)),
	d_rotation(glm::mat4(1.f)),
	d_translation(glm::mat4(1.f)),
	d_scale(glm::mat4(1.f)),
	d_hasShadow(true),
	d_deleteMe(false)
{
	if (model != nullptr && model->getMesh() != nullptr)
	{
		d_box = model->getMesh()->getAABBox();
	}
	else
	{
		SPDLOG_ERROR("Failed to create renderable: missing model or mesh.");
	}
}

void Renderable::setModelMatrix(const glm::mat4& matrix)
{
	d_model = matrix;
}

void Renderable::setRotationMatrix(const glm::mat4& matrix)
{
	d_rotation = matrix;
}

void Renderable::setTranslationMatrix(const glm::mat4& matrix)
{
	d_translation = matrix;
	updateAABBoxPosition();
}

void Renderable::updateAABBoxPosition()
{
	if (d_modelPtr == nullptr || d_modelPtr->getMesh() == nullptr)
	{
		return;
	}
	d_box.setMid(glm::vec3(d_translation * glm::vec4(d_modelPtr->getMesh()->getAABBox().getMid(), 1.f)));
}

void Renderable::updateAABBoxSize()
{
	if (d_modelPtr == nullptr || d_modelPtr->getMesh() == nullptr)
	{
		return;
	}
	d_box.setHalfWidths(glm::vec3(d_scale * glm::vec4(d_modelPtr->getMesh()->getAABBox().getHalfWidths(), 1.f)));
}

void Renderable::setScaleMatrix(const glm::mat4& matrix)
{
	d_scale = matrix;
	updateAABBoxSize();
}

void Renderable::draw(const std::shared_ptr<Renderer>& renderer, const Camera&) const
{
	if (d_modelPtr == nullptr || d_modelPtr->getMesh() == nullptr)
	{
		return;
	}
	renderer->draw(	getModel()->getVertexArrayObject(), 
					static_cast<int>(getModel()->getMesh()->getVertexBuffer()->getSize() / 3), 
					getModel()->getMaterial(), 
					d_model,
					d_rotation,
					d_translation,
					d_scale,
					d_hasShadow);
}

Model* Renderable::getModel() const
{
	return d_modelPtr;
}

const AABBox& Renderable::getAABBox() const
{
	return d_box;
}

bool Renderable::deleteRequested() const
{
	return d_deleteMe;
}

void Renderable::markDeleted()
{
	d_deleteMe = true;
}

void Renderable::setShadow(bool hasShadow)
{
	d_hasShadow = hasShadow;
}
