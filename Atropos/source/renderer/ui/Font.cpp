#include "renderer/ui/Font.h"

#include "io/FileLoader.h"
#include "resource/Image.h"

#include <sstream>
#include <stdexcept>
#include <vector>

Font::Font()
	:
	d_atlas(0),
	d_atlasWidth(0),
	d_atlasHeight(0),
	d_size(0),
	d_lineHeight(0)
{
}

Font::~Font()
{
	// Guarded so an uninitialised font can be destroyed without a context.
	if (d_atlas != 0)
	{
		glDeleteTextures(1, &d_atlas);
	}
}

void Font::load(const std::string& metricsFile, const std::string& atlasFile)
{
	FileLoader loader(metricsFile);
	loader.load();

	std::istringstream stream(loader.getContent());
	std::string line;

	if (!std::getline(stream, line) || line.empty() || line[0] != '#')
	{
		throw std::runtime_error("Font metrics are missing their header comment: " + metricsFile);
	}
	if (!std::getline(stream, line))
	{
		throw std::runtime_error("Font metrics are missing their header line: " + metricsFile);
	}

	std::istringstream header(line);
	header >> d_atlasWidth >> d_atlasHeight >> d_size >> d_lineHeight;
	if (d_atlasWidth <= 0 || d_atlasHeight <= 0 || d_size <= 0 || d_lineHeight <= 0)
	{
		throw std::runtime_error("Font metrics header is malformed: " + metricsFile);
	}

	while (std::getline(stream, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::istringstream entry(line);
		Glyph glyph;
		int code = 0;
		entry >> code >> glyph.atlasX >> glyph.atlasY >> glyph.width >> glyph.height
			>> glyph.offsetX >> glyph.offsetY >> glyph.advance;

		if (code >= 0 && code < 256)
		{
			d_glyphs[static_cast<std::size_t>(code)] = glyph;
		}
	}

	Image image;
	image.load(atlasFile);

	if (static_cast<int>(image.getWidth()) != d_atlasWidth || static_cast<int>(image.getHeight()) != d_atlasHeight)
	{
		throw std::runtime_error("Font atlas size does not match the metrics: " + atlasFile);
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &d_atlas);
	glTextureStorage2D(d_atlas, 1, GL_RGBA8, d_atlasWidth, d_atlasHeight);
	glTextureSubImage2D(d_atlas, 0, 0, 0, d_atlasWidth, d_atlasHeight, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());

	// No mipmaps and no wrapping: glyphs are magnified at integer scales and
	// neighbouring cells must never bleed in.
	glTextureParameteri(d_atlas, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(d_atlas, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(d_atlas, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(d_atlas, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLuint Font::getAtlas() const
{
	return d_atlas;
}

int Font::getAtlasWidth() const
{
	return d_atlasWidth;
}

int Font::getAtlasHeight() const
{
	return d_atlasHeight;
}

int Font::getLineHeight() const
{
	return d_lineHeight;
}

int Font::getSize() const
{
	return d_size;
}

float Font::getAdvance(unsigned char character) const
{
	return d_glyphs[character].advance;
}

bool Font::getGlyph(unsigned char character, GLuint& atlasX, GLuint& atlasY,
	GLuint& width, GLuint& height, int& offsetX, int& offsetY) const
{
	const Glyph& glyph = d_glyphs[character];
	if (glyph.width == 0 || glyph.height == 0)
	{
		return false;
	}

	atlasX = glyph.atlasX;
	atlasY = glyph.atlasY;
	width = glyph.width;
	height = glyph.height;
	offsetX = glyph.offsetX;
	offsetY = glyph.offsetY;
	return true;
}