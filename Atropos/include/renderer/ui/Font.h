#pragma once

#include <glad/gl.h>

#include <array>
#include <string>

/**
* \brief Bitmap font for the user interface. Glyph rectangles are in atlas
* texels; a glyph is drawn at its ink box top-left corner.
*/
class Font
{
public:
	Font();

	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	~Font();

	/**
	* \brief Loads the metrics file and its atlas.
	 * Throws std::runtime_error if either cannot be read.
	*/
	void load(const std::string& metricsFile, const std::string& atlasFile);

	GLuint getAtlas() const;

	int getAtlasWidth() const;

	int getAtlasHeight() const;

	/** \brief Distance between consecutive baselines, in texels. */
	int getLineHeight() const;

	/** \brief Size the glyphs were rasterised at, in texels. */
	int getSize() const;

	float getAdvance(unsigned char character) const;

	/**
	* \brief Looks up a glyph. Returns false when the character has no ink,
	* which happens for the space and for anything outside the atlas.
	*/
	bool getGlyph(unsigned char character, GLuint& atlasX, GLuint& atlasY,
		GLuint& width, GLuint& height, int& offsetX, int& offsetY) const;

private:
	struct Glyph
	{
		GLuint atlasX = 0;
		GLuint atlasY = 0;
		GLuint width = 0;
		GLuint height = 0;
		int offsetX = 0;
		int offsetY = 0;
		float advance = 0.f;
	};

	GLuint d_atlas;
	int d_atlasWidth;
	int d_atlasHeight;
	int d_size;
	int d_lineHeight;
	std::array<Glyph, 256> d_glyphs;
};