#include "CRenderableProxy.h"
#include "CRenderable.h"

CRenderableProxy::CRenderableProxy(CRenderable* renderable)
	:
	d_renderable(renderable)
{
	return;
}

CRenderableProxy::~CRenderableProxy()
{
	// Proxy destroyed, request resource deletion
	d_renderable->markDeleted();
}

void CRenderableProxy::setRotationMatrix(const glm::mat4& matrix)
{
	d_renderable->setRotationMatrix(matrix);
}

void CRenderableProxy::setTranslationMatrix(const glm::mat4& matrix)
{
	d_renderable->setTranslationMatrix(matrix);
}

void CRenderableProxy::setScaleMatrix(const glm::mat4& matrix)
{
	d_renderable->setScaleMatrix(matrix);
}

bool CRenderableProxy::isVisible() const
{
	return d_renderable->isVisible();
}
