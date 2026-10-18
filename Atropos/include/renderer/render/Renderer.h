#pragma once

class VertexArrayObject;
class AMaterial;
class Shader;
class Mesh;

#include "renderer/resource/VertexArrayObject.h"
#include "renderer/render/Bloom.h"
#include "renderer/render/Explosion.h"
#include "renderer/render/RenderTarget.h"
#include "renderer/render/ShadowBuffer.h"
#include "renderer/render/Sky.h"
#include "renderer/ui/UI.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>

class RenderRequest
{
public:
	RenderRequest(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow);

	const VertexArrayObject* getVAO() const;

	int getSize() const;

	const AMaterial* getMaterial() const;
	
	const glm::mat4& getModel() const;

	const glm::mat4& getRotation() const;

	const glm::mat4& getTranslation() const;

	const glm::mat4& getScale() const;

	bool hasShadow() const;

private:
	const VertexArrayObject* d_vao;
	int d_size; /**< Vertex count. */
	
	const AMaterial* d_material;
	bool d_shadow;

	glm::mat4 d_model;
	glm::mat4 d_rotation;
	glm::mat4 d_translation;
	glm::mat4 d_scale;
};

struct FrameStats
{
	unsigned int drawCalls = 0;
	unsigned int vertexCount = 0;
};

class Renderer
{
public:
	enum Stage { Start, Draw, End };

	Renderer(int width, int height, Shader* shadow, const std::string& resourcePath);

	~Renderer();

	/**
	* \brief Resizes the viewport to the new window size.
	*/
	void onResize(int width, int height);

	/**
	* \brief Signals start of new frame; resets frame statistics.
	*/
	void startFrame();

	/**
	* \brief Signals the end of the frame's draw calls; runs the shadow, geometry and
	* effect passes. dtime is in seconds and drives the effect simulation.
	*/
	void endFrame(float dtime);

	void setViewMatrix(const glm::mat4& view);
	
	void setProjectionMatrix(const glm::mat4& proj);

	void setClearColor(const glm::vec3& color);

	/**
	* \brief Distance fog. Geometry fades to the horizon colour between the two
	* distances so a large flat plane recedes instead of ending in a hard edge.
	*/
	void setFog(const glm::vec3& color, float start, float end);

	/**
	* \brief Turns the background gradient on or off and sets its colours. Off by
	* default, so a scene that sets a flat background colour keeps it. Enabling
	* the gradient also points the fog at the horizon colour, otherwise the two
	* would meet in a visible seam.
	*/
	void setSky(bool enabled, const glm::vec3& zenith, const glm::vec3& horizon);

	/**
	* \brief Adds the object to the render requests.
	*/
	void draw(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow);
	
	/**
	* \brief Quick workaround: the renderer owns the light position.
	*/
	void setLightPosition(const glm::vec3& position);

	/**
	* \brief Starts a fireball burst. The effect owns its particles, so it is not
	* tied to a game object and needs no render request.
	*/
	void addExplosion(const glm::vec3& position);

/**
	* \brief Screen space drawing queue for the current frame.
	*/
	UI& ui();

private:
	/** \brief Binds the default framebuffer and clears it. */
	void clearTarget();

	/** \brief Draws the background gradient. */
	void skyPass();

	/** \brief Adds the bloom halo and presents the offscreen scene to the screen. */
	void postProcess();

	void geometryPass();

	void effectPass(float dtime);

	void shadowPass();

	/**
	* \brief Stores the render requests for the current frame.
	*/
	std::vector<RenderRequest> d_renderRequests;

	Shader* d_shadowShader;
	ShadowBuffer d_shadowBuffer;
	Sky d_sky;
	Explosion d_explosions;
	Bloom d_bloom;

	/** \brief Half float scene buffer, so bright effects have headroom to bloom. */
	std::unique_ptr<RenderTarget> d_sceneTarget;

	Stage d_stage;

	int d_width;
	int d_height;

	glm::vec3 d_clearColor;

	glm::vec3 d_fogColor;
	float d_fogStart;
	float d_fogEnd;

	glm::mat4 d_view;
	glm::mat4 d_proj;

	glm::vec3 d_lightPosition;
	glm::mat4 d_depthView;
	glm::mat4 d_depthProj;

	FrameStats d_frameStats;

	UI d_ui;
};
