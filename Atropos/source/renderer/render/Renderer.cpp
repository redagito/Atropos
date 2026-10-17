#include "renderer/render/Renderer.h"

#include "renderer/resource/Shader.h"
#include "renderer/resource/AMaterial.h"
#include "renderer/resource/VertexArrayObject.h"
#include "renderer/resource/Model.h"

#include "debug/Log.h"
#include "debug/Profiler.h"

#include <glm/ext.hpp>
#include <stdexcept>

// Initialize values, immutable class
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
	d_shadowShader(shadow), // Shadow shader
	d_stage(Renderer::End), // Initial stage
	d_width(width), // Screen width
	d_height(height), // Screen height
	d_clearColor(1.f), // Default white
	d_view(glm::mat4(1.f)), // Active view matrix
	d_proj(glm::mat4(1.f)), // Active projection matrix
	d_lightPosition(0.f, 100.f, 50.f) // The sun, TODO remove from here!
{
	LOG_INFO("Created renderer.");
	
	// Depth matrices for shadow mapping
	d_depthProj = glm::ortho(-50.f, 50.f, -50.f, 50.f, 1.f,120.f);
	d_depthView = glm::lookAt(d_lightPosition, glm::vec3(0.f, 0.f, 50.f), glm::vec3(0.f, 0.f, 1.f));

	// Background color
	glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, 0.f);
	
	// Depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Winding order, standard is counter-clockwise
	glFrontFace(GL_CCW);

	// Enable blending

	// Initial viewport
	glViewport(0, 0, width, height);

	// Init request storage
	d_renderRequests.reserve(500); // Should be enough
}

Renderer::~Renderer()
{
	LOG_INFO("Destroyed renderer.");
}

// Pre render call for render effects
void Renderer::startFrame()
{
	// Clear render queue
	d_renderRequests.clear();

	// Reset statistics
	d_frameStats.drawCalls = 0;
	d_frameStats.vertexCount = 0;

	d_stage = Renderer::Start;
}

void Renderer::endFrame()
{
	Profiler profiler("end_frame");
	d_stage = Renderer::End;
	
	// Draw to shadow map texture
	shadowPass();

	// Draw geometry
	geometryPass();

	// Screen effects etc get applied here
	
	// TODO User interface 2D drawing
}

// Stores draw calls as render requests
void Renderer::draw(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow)
{
	// Fills draw request array
	if (d_stage == Renderer::Start || d_stage == Renderer::Draw)
	{
		d_stage = Renderer::Draw;
	}
	else
	{
		LOG_ERROR("Called Renderer.draw while in end state");
		throw std::runtime_error("Renderer.draw: Invalid call to draw without calling startFrame.");
	}
	// Add new render request
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

	// Shadow bias matrix
	static glm::mat4 biasMatrix(0.5f, 0.f, 0.f, 0.f,
								0.f, 0.5f, 0.f, 0.f,
								0.f, 0.f, 0.5f, 0.f,
								0.5f, 0.5f, 0.5f, 1.f);

	// Bind default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Reset viewport
	glViewport(0, 0, d_width, d_height);

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Precalculate part of depth bias matrix
	glm::mat4 depthBias = biasMatrix * d_depthProj * d_depthView;

	// Draw buffered render calls
	for (auto& request : d_renderRequests)
	{
		// Texture unit 0 used by material
		glActiveTexture(GL_TEXTURE0);
		const AMaterial* material = request.getMaterial();
		
		// Set material as active
		// This sets shader uniforms for visual appearance
		// TODO: Material should only store visual data and no shader object, 
		// the shader should be picked and initialized by the renderer based on the material.  
		material->setActive();

		// Set transformation matrices for model rendering
		// Precomputed model matrix
		material->getShader()->setShaderValueMat4("model", request.getModel());
		// Rotation
		material->getShader()->setShaderValueMat4("rotation", request.getRotation());
		// Translation
		material->getShader()->setShaderValueMat4("translation", request.getTranslation());
		// Scale
		material->getShader()->setShaderValueMat4("scale", request.getScale());
		
		// Set viewing values
		// TODO: Provide camera object, which holds these values instead of copying them?
		
		// Projection
		material->getShader()->setShaderValueMat4("proj", d_proj);
		// View
		material->getShader()->setShaderValueMat4("view", d_view);
		
		// Set uniforms for simple shadow mapping
		// Depth bias
		material->getShader()->setShaderValueMat4("bias", depthBias * request.getModel());
		
		// Inverse light direction
		// TODO: The direction should be stored in a directional light object and not inside the renderer.
		material->getShader()->setShaderValueVec3("lightInvDir", d_lightPosition);
		
		// Set texture unit 1 active for depth texture for shadow mapping
		// TODO: Rework this for multitexturing
		glActiveTexture(GL_TEXTURE1);
		
		// Bind depth texture from shadow FBO to texture unit 1
		glBindTexture(GL_TEXTURE_2D, d_shadowBuffer.getDepthTexture());
		
		// Send texture unit 1, depth texture, to shader
		// TODO: Find some way to automate this "set active texture unit" -> "bind texture" -> "send texture to shader" thing.
		material->getShader()->setShaderTexture("shadow", 1);

		// Set vertex data buffers active by VAO
		request.getVAO()->setActive();

		// Render call
		// TODO: Remove hardcoded triangle enum, store primitive type in mesh?
		// TODO: Should request.getSize() provide number of vertices or number of primitives?
		glDrawArrays(GL_TRIANGLES, 0, request.getSize());
		d_frameStats.drawCalls++;
	
		// Disable VAO
		// TODO: really necessary?
		request.getVAO()->setInactive();

		// Add vertex stats
		d_frameStats.vertexCount += request.getSize();
	}
	return;
}

// TODO use framebuffer class for this?
void Renderer::shadowPass()
{
	Profiler profiler("shadow_pass");

	// Bind shadow buffer
	d_shadowBuffer.setActive();

	// Recalculate depth view.
	d_depthView = glm::lookAt(d_lightPosition, glm::vec3(0.f, 0.f, d_lightPosition.z), glm::vec3(0.f, 0.f, 1.f));

	// Cull front facing, only draw back facing
	glCullFace(GL_FRONT);
	
	// Clear depth buffer of shadow FBO
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// Set shadow shader active
	d_shadowShader->setActive();

	// Iterate over render calls and draw depth to shadow map
	for (auto& request : d_renderRequests)
	{
		// Only process shadow creating objects
		// TODO: hasShadow should be stored in material?
		if (request.hasShadow())
		{
			// Shader uniforms
			// Transformation matrices
			d_shadowShader->setShaderValueMat4("model", request.getModel());
			
			// View matrices
			d_shadowShader->setShaderValueMat4("view", d_depthView);
			d_shadowShader->setShaderValueMat4("proj", d_depthProj);

			// VAO management
			request.getVAO()->setActive();

			// Render call
			glDrawArrays(GL_TRIANGLES, 0, request.getSize());
			d_frameStats.drawCalls++;
	
			// Disable VAO
			request.getVAO()->setInactive();

			// Add vertex stats
			d_frameStats.vertexCount += request.getSize();
		}
	}
	// Reset face culling state
	glCullFace(GL_BACK);
}

// Sets light position, TODO remove and implement proper light system.
void Renderer::setLightPosition(const glm::vec3& position) 
{
	d_lightPosition = position;
}
