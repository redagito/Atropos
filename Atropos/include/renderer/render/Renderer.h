#pragma once

// Forward declares
class VertexArrayObject;
class AMaterial;
class Shader;
class Mesh;

#include "renderer/resource/VertexArrayObject.h"
#include "renderer/render/ShadowBuffer.h"

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <vector>

/**
* \brief Represents a render request.
*/
class RenderRequest
{
public:
	/**
	* \brief Stores object parameters for drawing.
	*/
	RenderRequest(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow);

	/**
	* \brief Returns vertex array object.
	*/
	const VertexArrayObject* getVAO() const;

	/**
	* \brief Returns vertex count.
	*/
	int getSize() const;

	/**
	* \brief Returns material.
	*/
	const AMaterial* getMaterial() const;
	
	/**
	* \brief Returns model matrix
	*/
	const glm::mat4& getModel() const;

	/**
	* \brief Returns rotation matrix
	*/
	const glm::mat4& getRotation() const;

	/**
	* \brief Returns translation matrix
	*/
	const glm::mat4& getTranslation() const;

	/**
	* \brief Returns scale matrix
	*/
	const glm::mat4& getScale() const;

	/**
	* \brief Returns whether the object creates shadows.
	*/
	bool hasShadow() const;

private:
	const VertexArrayObject* d_vao; /**< Vertex array object. */
	int d_size; /**< Vertex count. */
	
	const AMaterial* d_material; /**< Rendering material. */
	bool d_shadow; /**< Has shadow. */

	glm::mat4 d_model; /**< Model matrix. */
	glm::mat4 d_rotation; /**< Rotation matrix. */
	glm::mat4 d_translation; /**< Translation matrix. */
	glm::mat4 d_scale; /**< Scale matrix. */
};

/**
* \brief Per frame statistics.
*/
struct FrameStats
{
	unsigned int drawCalls = 0;
	unsigned int vertexCount = 0;
};

/**
* \brief Provides rendering functionality.
*/
class Renderer
{
public:
	/** 
	* \brief Possible render stages.
	*/
	enum Stage { Start, Draw, End };

	/**
	* \brief Creates renderer and sets viewport size and shadow shader.
	*/
	Renderer(int width, int height, Shader* shadow);

	/**
	* \brief Deletes renderer.
	*/
	~Renderer();

	/**
	* \brief Window resize callback.
	*
	* Resizes viewport to the new window size.
	*/
	void onResize(int width, int height);

	/**
	* \brief Signals start of new frame.
	*
	* This should be called at the start of a new frame and before
	* any draw calls are made. Resets frame statistics.
	*/
	void startFrame();

	/**
	* \brief Signals end of draw calls for the current frame.
	*
	* Call after all draw calls for the current frame are made. Is used
	* for post processing.
	*/
	void endFrame();

	/** 
	* \brief Sets view matrix for rendering.
	*/
	void setViewMatrix(const glm::mat4& view);
	
	/**
	* \brief Sets projection matrix.
	*/
	void setProjectionMatrix(const glm::mat4& proj);

	/**
	 * \brief Sets clear color for color buffer.
	 */
	void setClearColor(const glm::vec3& color);

	/**
	* \brief Adds the object to the render requests.
	*/
	void draw(const VertexArrayObject* vao, int size, const AMaterial* material, const glm::mat4& model, const glm::mat4& rotation, const glm::mat4& translation, const glm::mat4& scale, bool hasShadow);
	
	/**
	* \brief Sets light poisition.
	*
	* Quick workaround for shadow mapping. The renderer should not store the lights or have any knowledge of light movement.
	*/
	void setLightPosition(const glm::vec3& position);

private:
	/**
	* \brief Draws geometry from all stored render requests.
	*/
	void geometryPass();

	/**
	* \brief Creates shadow texture.
	*/
	void shadowPass();

	/**
	* \brief Stores the render requests for the current frame.
	*/
	std::vector<RenderRequest> d_renderRequests;

	Shader* d_shadowShader; /**< Shadow mapping shader. */
	ShadowBuffer d_shadowBuffer; /**< FBO for shadow mapping. */

	Stage d_stage; /**< Current rendering stage. */

	int d_width; /**< Current draw area width. */
	int d_height; /**< Current draw area height. */

	glm::vec3 d_clearColor; /**< Current clear color for color buffer. */

	glm::mat4 d_view; /**< Active view matrix. */
	glm::mat4 d_proj; /**< Active projection matrix. */

	glm::vec3 d_lightPosition; /**< Light position. */
	glm::mat4 d_depthView; /**< Depth view matrix. */
	glm::mat4 d_depthProj; /**< Depth projection matrix. */

	FrameStats d_frameStats; /**< Per frame rendering statistics. */
};
