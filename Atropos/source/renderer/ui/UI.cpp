#include "renderer/ui/UI.h"

#include "io/FileLoader.h"
#include "renderer/resource/ShaderSources.h"

#include <cstddef>
#include <stdexcept>

UI::UI()
	:
	d_buffer(0),
	d_vao(0),
	d_useAtlas(-1),
	d_atlasSampler(-1),
	d_width(1),
	d_height(1),
	d_fps(0.f)
{
}

UI::~UI()
{
	// Guarded so an uninitialised instance can be destroyed without a context.
	if (d_vao != 0)
	{
		glDeleteVertexArrays(1, &d_vao);
	}
	if (d_buffer != 0)
	{
		glDeleteBuffers(1, &d_buffer);
	}
}

void UI::init(const std::string& resourcePath)
{
	d_font.load(resourcePath + "font/DejaVuSansMono.fnt", resourcePath + "font/DejaVuSansMono.tga");

	FileLoader vertexLoader(resourcePath + "shader/src/ui.vs");
	vertexLoader.load();
	FileLoader fragmentLoader(resourcePath + "shader/src/ui.fs");
	fragmentLoader.load();

	ShaderSources sources;
	sources.vertex = vertexLoader.getContent();
	sources.fragment = fragmentLoader.getContent();
	d_shader.init(sources);

	d_useAtlas = d_shader.getUniformLocation("useAtlas");
	d_atlasSampler = d_shader.getUniformLocation("atlas");

	glCreateBuffers(1, &d_buffer);
	glCreateVertexArrays(1, &d_vao);
	glVertexArrayVertexBuffer(d_vao, 0, d_buffer, 0, sizeof(Vertex));

	const struct { GLuint location; GLint components; std::size_t offset; } attributes[] =
	{
		{0, 2, offsetof(Vertex, position)},
		{1, 2, offsetof(Vertex, uv)},
		{2, 4, offsetof(Vertex, color)},
	};

	for (const auto& attribute : attributes)
	{
		glVertexArrayAttribFormat(d_vao, attribute.location, attribute.components, GL_FLOAT, GL_FALSE, static_cast<GLuint>(attribute.offset));
		glVertexArrayAttribBinding(d_vao, attribute.location, 0);
		glEnableVertexArrayAttrib(d_vao, attribute.location);
	}
}

void UI::onResize(int width, int height)
{
	d_width = width > 0 ? width : 1;
	d_height = height > 0 ? height : 1;
}

void UI::beginFrame()
{
	d_solid.clear();
	d_text.clear();
}

void UI::flush()
{
	if (d_solid.empty() && d_text.empty())
	{
		return;
	}

	const std::size_t total = d_solid.size() + d_text.size();
	std::vector<Vertex> combined;
	combined.reserve(total);
	combined.insert(combined.end(), d_solid.begin(), d_solid.end());
	combined.insert(combined.end(), d_text.begin(), d_text.end());

	glNamedBufferData(d_buffer, static_cast<GLsizeiptr>(combined.size() * sizeof(Vertex)), combined.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(d_vao);
	d_shader.setActive();
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (d_atlasSampler != -1)
	{
		glBindTextureUnit(0, d_font.getAtlas());
		d_shader.setShaderTexture(d_atlasSampler, 0);
	}

	const GLint solidCount = static_cast<GLint>(d_solid.size());
	if (solidCount > 0)
	{
		d_shader.setShaderValueFloat(d_useAtlas, 0.f);
		glDrawArrays(GL_TRIANGLES, 0, solidCount);
	}

	const GLint textCount = static_cast<GLint>(d_text.size());
	if (textCount > 0)
	{
		d_shader.setShaderValueFloat(d_useAtlas, 1.f);
		glDrawArrays(GL_TRIANGLES, solidCount, textCount);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void UI::drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	const glm::vec2 topLeft = toClip(position.x, position.y);
	const glm::vec2 bottomRight = toClip(position.x + size.x, position.y + size.y);

	d_solid.push_back({topLeft, glm::vec2(0.f), color});
	d_solid.push_back({glm::vec2(bottomRight.x, topLeft.y), glm::vec2(0.f), color});
	d_solid.push_back({glm::vec2(topLeft.x, bottomRight.y), glm::vec2(0.f), color});
	d_solid.push_back({glm::vec2(topLeft.x, bottomRight.y), glm::vec2(0.f), color});
	d_solid.push_back({glm::vec2(bottomRight.x, topLeft.y), glm::vec2(0.f), color});
	d_solid.push_back({bottomRight, glm::vec2(0.f), color});
}

void UI::drawText(const glm::vec2& position, const std::string& text, float scale, const glm::vec4& color)
{
	const float atlasWidth = static_cast<float>(d_font.getAtlasWidth());
	const float atlasHeight = static_cast<float>(d_font.getAtlasHeight());

	float penX = position.x;
	for (const char character : text)
	{
		const unsigned char code = static_cast<unsigned char>(character);

		GLuint atlasX = 0;
		GLuint atlasY = 0;
		GLuint width = 0;
		GLuint height = 0;
		int offsetX = 0;
		int offsetY = 0;

		if (d_font.getGlyph(code, atlasX, atlasY, width, height, offsetX, offsetY))
		{
			const float x0 = penX + static_cast<float>(offsetX) * scale;
			const float y0 = position.y + static_cast<float>(offsetY) * scale;
			const float x1 = x0 + static_cast<float>(width) * scale;
			const float y1 = y0 + static_cast<float>(height) * scale;

			const glm::vec2 uv0(static_cast<float>(atlasX) / atlasWidth, static_cast<float>(atlasY) / atlasHeight);
			const glm::vec2 uv1(static_cast<float>(atlasX + width) / atlasWidth, static_cast<float>(atlasY + height) / atlasHeight);

			const glm::vec2 topLeft = toClip(x0, y0);
			const glm::vec2 bottomRight = toClip(x1, y1);

			d_text.push_back({topLeft, uv0, color});
			d_text.push_back({glm::vec2(bottomRight.x, topLeft.y), glm::vec2(uv1.x, uv0.y), color});
			d_text.push_back({glm::vec2(topLeft.x, bottomRight.y), glm::vec2(uv0.x, uv1.y), color});
			d_text.push_back({glm::vec2(topLeft.x, bottomRight.y), glm::vec2(uv0.x, uv1.y), color});
			d_text.push_back({glm::vec2(bottomRight.x, topLeft.y), glm::vec2(uv1.x, uv0.y), color});
			d_text.push_back({bottomRight, uv1, color});
		}

		penX += d_font.getAdvance(code) * scale;
	}
}

float UI::measureText(const std::string& text, float scale) const
{
	float width = 0.f;
	for (const char character : text)
	{
		width += d_font.getAdvance(static_cast<unsigned char>(character)) * scale;
	}
	return width;
}

int UI::getWidth() const
{
	return d_width;
}

void UI::setFrameTime(float dtime)
{
	if (dtime <= 0.f)
	{
		return;
	}

	const float instant = 1.f / dtime;
	// Smoothed so the readout does not flicker with every frame.
	d_fps = d_fps <= 0.f ? instant : d_fps + (instant - d_fps) * 0.1f;
}

int UI::getFps() const
{
	return static_cast<int>(d_fps + 0.5f);
}

int UI::getHeight() const
{
	return d_height;
}

glm::vec2 UI::toClip(float x, float y) const
{
	return glm::vec2(
		(x / static_cast<float>(d_width)) * 2.f - 1.f,
		1.f - (y / static_cast<float>(d_height)) * 2.f);
}