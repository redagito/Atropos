#include "renderer/render/Renderer.h"

#include "renderer/resource/Shader.h"
#include "renderer/resource/AMaterial.h"
#include "renderer/resource/VertexArrayObject.h"
#include "renderer/resource/Model.h"

#include <spdlog/spdlog.h>
#include "debug/Profiler.h"

#include <glm/ext.hpp>
#include <stdexcept>

RenderRequest::RenderRequest(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool shadow)
	:	
	d_vao(vao),
	d_size(size),
	d_material(material),
	d_shadow(shadow),
	d_model(model),
	d_rotation(rotation),
	d_translation(translation),
	d_scale(scale)
{
	return;
}

const VertexArrayObject* RenderRequest::getVAO() const
{
	return d_vao;
}

int RenderRequest::getSize() const
{
	return d_size;
}

const AMaterial* RenderRequest::getMaterial() const
{
	return d_material;
}

const glm::mat4& RenderRequest::getModel() const
{
	return d_model;
}

const glm::mat4& RenderRequest::getRotation() const
{
	return d_rotation;
}

const glm::mat4& RenderRequest::getTranslation() const
{
	return d_translation;
}

const glm::mat4& RenderRequest::getScale() const
{
	return d_scale;
}

bool RenderRequest::hasShadow() const
{
	return d_shadow;
}

Renderer::Renderer(int width, int height, Shader* shadow)
	:	
	d_shadowShader(shadow),
	d_stage(Renderer::End),
	d_width(width),
	d_height(height),
	d_clearColor(1.f),
	d_view(glm::mat4(1.f)),
	d_proj(glm::mat4(1.f)),
	d_lightPosition(0.f, 100.f, 50.f)
{
	SPDLOG_INFO("Created renderer.");
	
	d_depthProj = glm::ortho(-50.f, 50.f, -50.f, 50.f, 1.f,120.f);
	d_depthView = glm::lookAt(d_lightPosition, glm::vec3(0.f, 0.f, 50.f), glm::vec3(0.f, 0.f, 1.f));

	glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, 0.f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Winding order, standard is counter-clockwise
	glFrontFace(GL_CCW);

	glViewport(0, 0, width, height);

	d_renderRequests.reserve(500);
}

Renderer::~Renderer()
{
	SPDLOG_INFO("Destroyed renderer.");
}

void Renderer::startFrame()
{
	d_renderRequests.clear();

	d_frameStats.drawCalls = 0;
	d_frameStats.vertexCount = 0;

	d_stage = Renderer::Start;
}

void Renderer::endFrame()
{
	Profiler profiler("end_frame");
	d_stage = Renderer::End;
	
	shadowPass();

	geometryPass();

	// TODO User interface 2D drawing
}

void Renderer::draw(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow)
{
	if (d_stage == Renderer::Start || d_stage == Renderer::Draw)
	{
		d_stage = Renderer::Draw;
	}
	else
	{
		throw std::runtime_error("Renderer.draw: Invalid call to draw without calling startFrame.");
	}
	d_renderRequests.push_back(RenderRequest(vao, size, material, model, rotation, translation, scale, hasShadow));
}

void Renderer::setViewMatrix(const glm::mat4& view)
{
	d_view = view;
}

void Renderer::setProjectionMatrix(const glm::mat4& proj)
{
	d_proj = proj;
}

void Renderer::setClearColor(const glm::vec3& color)
{
	if (d_clearColor != color)
	{
		d_clearColor = color;
		glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, 0.f);
	}
}

void Renderer::onResize(int width, int height)
{
	d_width = width;
	d_height = height;
	glViewport(0, 0, d_width, d_height);
}

void Renderer::geometryPass()
{
	Profiler profiler("geometry_pass");

	static glm::mat4 biasMatrix(0.5f, 0.f, 0.f, 0.f,
								0.f, 0.5f, 0.f, 0.f,
								0.f, 0.f, 0.5f, 0.f,
								0.5f, 0.5f, 0.5f, 1.f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glViewport(0, 0, d_width, d_height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 depthBias = biasMatrix * d_depthProj * d_depthView;

	for (auto& request : d_renderRequests)
	{
		// Texture unit 0 used by material
		glActiveTexture(GL_TEXTURE0);
		const AMaterial* material = request.getMaterial();
		
		// TODO: let the renderer pick the shader from the material.
		material->setActive();

		material->getShader()->setShaderValueMat4("model", request.getModel());
		material->getShader()->setShaderValueMat4("rotation", request.getRotation());
		material->getShader()->setShaderValueMat4("translation", request.getTranslation());
		material->getShader()->setShaderValueMat4("scale", request.getScale());
		
		// TODO: pass a camera object instead of copying view and proj values.
		
		material->getShader()->setShaderValueMat4("proj", d_proj);
		material->getShader()->setShaderValueMat4("view", d_view);
		
		material->getShader()->setShaderValueMat4("bias", depthBias * request.getModel());
		
		material->getShader()->setShaderValueVec3("lightInvDir", d_lightPosition);
		
		// Texture unit 1 carries the depth texture
		// TODO: rework the texture binding for multitexturing.
		glActiveTexture(GL_TEXTURE1);
		
		glBindTexture(GL_TEXTURE_2D, d_shadowBuffer.getDepthTexture());
		
		// TODO: automate bind-texture-then-bind-shader-unit.
		material->getShader()->setShaderTexture("shadow", 1);

		request.getVAO()->setActive();

		// TODO: remove hardcoded triangle enum, store primitive type in mesh.
		// TODO: clarify whether getSize() is vertices or primitives.
		glDrawArrays(GL_TRIANGLES, 0, request.getSize());
		d_frameStats.drawCalls++;
	
		// TODO: really necessary?
		request.getVAO()->setInactive();

		d_frameStats.vertexCount += request.getSize();
	}
	return;
}

// TODO use framebuffer class for this?
void Renderer::shadowPass()
{
	Profiler profiler("shadow_pass");

	d_shadowBuffer.setActive();

	d_depthView = glm::lookAt(d_lightPosition, glm::vec3(0.f, 0.f, d_lightPosition.z), glm::vec3(0.f, 0.f, 1.f));

	// Cull front facing, only draw back facing
	glCullFace(GL_FRONT);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	d_shadowShader->setActive();

	for (auto& request : d_renderRequests)
	{
		if (request.hasShadow())
		{
			d_shadowShader->setShaderValueMat4("model", request.getModel());
			
			d_shadowShader->setShaderValueMat4("view", d_depthView);
			d_shadowShader->setShaderValueMat4("proj", d_depthProj);

			request.getVAO()->setActive();

			glDrawArrays(GL_TRIANGLES, 0, request.getSize());
			d_frameStats.drawCalls++;
	
			request.getVAO()->setInactive();

			d_frameStats.vertexCount += request.getSize();
		}
	}
	glCullFace(GL_BACK);
}

// TODO: remove and implement a proper light system.
void Renderer::setLightPosition(const glm::vec3& position) 
{
	d_lightPosition = position;
}
