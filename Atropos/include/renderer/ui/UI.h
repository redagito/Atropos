#pragma once

#include "renderer/ui/Font.h"
#include "renderer/resource/Shader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

/**
* \brief Immediate mode drawing in pixel coordinates with the origin at the top
* left. Rectangles and text are queued separately and flushed with at most two
* draw calls, so callers can interleave them freely.
*/
class UI
{
public:
	UI();

	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;

	~UI();

	/**
	* \brief Loads the font and shader.
	 * Throws std::runtime_error when either is missing.
	*/
	void init(const std::string& resourcePath);

	void onResize(int width, int height);

	/** \brief Clears the queues; call before drawing a frame. */
	void beginFrame();

	/** \brief Uploads and draws everything queued since beginFrame. */
	void flush();

	void drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

	/**
	* \brief Draws a string with its top left at position. A scale of one is the
	* size the glyphs were rasterised at; whole scales keep the text crisp.
	*/
	void drawText(const glm::vec2& position, const std::string& text, float scale, const glm::vec4& color);

	float measureText(const std::string& text, float scale) const;

	/** \brief Height of one line of text at a scale, including the descender. */
	float getTextHeight(float scale) const;

	/** \brief Feeds the frame duration; call once per frame for the FPS readout. */
	void setFrameTime(float dtime);

	int getFps() const;

	int getWidth() const;

	int getHeight() const;

private:
	struct Vertex
	{
		glm::vec2 position;
		glm::vec2 uv;
		glm::vec4 color;
	};

	glm::vec2 toClip(float x, float y) const;

	Font d_font;
	Shader d_shader;
	GLuint d_buffer;
	GLuint d_vao;
	GLint d_useAtlas;
	GLint d_atlasSampler;
	int d_width;
	int d_height;
	float d_fps;
	std::vector<Vertex> d_solid;
	std::vector<Vertex> d_text;
};