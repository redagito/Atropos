#include "CRenderer.h"

#include "renderer/resource/CShader.h"
#include "renderer/resource/AMaterial.h"
#include "renderer/resource/CVertexArrayObject.h"
#include "renderer/resource/CModel.h"

#include "debug/Log.h"
#include "debug/CProfiler.h"

#include <glm/ext.hpp>

// Initialize values, immutable class
CRenderRequest::CRenderRequest(const CVertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool shadow)
	:	
	d_vao(vao),
	d_size(size),
	d_material(material),
	d_model(model),
	d_rotation(rotation),
	d_translation(translation),
	d_scale(scale),
	d_shadow(shadow)
{
	return;
}

const CVertexArrayObject* CRenderRequest::getVAO() const
{
	return d_vao;
}

int CRenderRequest::getSize() const
{
	return d_size;
}

const AMaterial* CRenderRequest::getMaterial() const
{
	return d_material;
}

const glm::mat4& CRenderRequest::getModel() const
{
	return d_model;
}

const glm::mat4& CRenderRequest::getRotation() const
{
	return d_rotation;
}

const glm::mat4& CRenderRequest::getTranslation() const
{
	return d_translation;
}

const glm::mat4& CRenderRequest::getScale() const
{
	return d_scale;
}

bool CRenderRequest::hasShadow() const
{
	return d_shadow;
}

CRenderer::CRenderer(int width, int height, CShader* shadow)
	:	
	d_width(width), // Screen width
	d_height(height), // Screen height
	d_stage(CRenderer::End), // Initial stage
	d_clearColor(1.f), // Default white
	d_useWireframe(false), // Wireframe mode
	d_shadowShader(shadow), // Shadow shader
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
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initial viewport
	glViewport(0, 0, width, height);

	// Init request storage
	d_renderRequests.reserve(500); // Should be enough
}

CRenderer::~CRenderer()
{
	LOG_INFO("Destroyed renderer.");
}

void CRenderer::setWireframe(bool state)
{
	d_useWireframe = state;
	if (d_useWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

// Pre render call for render effects
void CRenderer::startFrame()
{
	// Clear render queue
	d_renderRequests.clear();

	// Reset statistics
	d_frameStats.drawCalls = 0;
	d_frameStats.vertexCount = 0;
	d_frameStats.drawRequests = 0;

	d_stage = CRenderer::Start;
}

void CRenderer::endFrame()
{
	CProfiler profiler("end_frame");
	d_stage = CRenderer::End;
	
	// Draw to shadow map texture
	shadowPass();

	// Draw geometry
	geometryPass();

	// Screen effects etc get applied here
	// postProcess();
	
	// TODO User interface 2D drawing
	// drawUI();
}

// Stores draw calls as render requests
void CRenderer::draw(const CVertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow)
{
	// Fills draw request array
	++d_frameStats.drawRequests;
	if (d_stage == CRenderer::Start || d_stage == CRenderer::Draw)
	{
		d_stage = CRenderer::Draw;
	}
	else
	{
		LOG_ERROR("Called CRenderer.draw while in end state");
		throw std::runtime_error("CRenderer.draw: Invalid call to draw without calling startFrame.");
	}
	// Add new render request
	d_renderRequests.push_back(CRenderRequest(vao, size, material, model, rotation, translation, scale, hasShadow));
}

void CRenderer::setViewMatrix(const glm::mat4& view)
{
	d_view = view;
}

void CRenderer::setProjectionMatrix(const glm::mat4& proj)
{
	d_proj = proj;
}

void CRenderer::setClearColor(const glm::vec3& color)
{
	if (d_clearColor != color)
	{
		d_clearColor = color;
		glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, 0.f);
	}
}

void CRenderer::onResize(int width, int height)
{
	d_width = width;
	d_height = height;
	glViewport(0, 0, d_width, d_height);
}

void CRenderer::geometryPass()
{
	CProfiler profiler("geometry_pass");

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
		// TODO: The direction should be stiored in a directional light object and not inside the renderer.
		material->getShader()->setShaderValueVec3("invLightDir", d_lightPosition);
		
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
		d_frameStats.vertexCount += request.getSize() * 3; // Triangles * 3 -> vertices
	}
	return;
}

// TODO use framebuffer class for this?
void CRenderer::shadowPass()
{
	CProfiler profiler("shadow_pass");

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
			d_frameStats.vertexCount += request.getSize() * 3; // Triangles * 3 -> vertices
		}
	}
	// Reset face culling state
	glCullFace(GL_BACK);
}

// Sets light position, TODO remove and implement proper light system.
void CRenderer::setLightPosition(const glm::vec3& position) 
{
	d_lightPosition = position;
}

const SFrameStats& CRenderer::getFrameStats() const
{
	return d_frameStats;
}
