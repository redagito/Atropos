#include "GLTestContext.h"
#include "renderer/ui/Font.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("The interface font loads its metrics and atlas", "[ui][font]")
{
	GLTestContext context;

	Font font;
	font.load(std::string(ATROPOS_FONT_DIR) + "/DejaVuSansMono.fnt",
		std::string(ATROPOS_FONT_DIR) + "/DejaVuSansMono.tga");

	CHECK(context.sink().errors == 0);
	CHECK(font.getAtlas() != 0);
	CHECK(font.getSize() == 32);
	CHECK(font.getLineHeight() > font.getSize());
	CHECK(font.getAtlasWidth() == 512);
	CHECK(font.getAtlasHeight() == 256);

	GLuint atlasX = 0;
	GLuint atlasY = 0;
	GLuint width = 0;
	GLuint height = 0;
	int offsetX = 0;
	int offsetY = 0;

	REQUIRE(font.getGlyph('0', atlasX, atlasY, width, height, offsetX, offsetY));
	CHECK(width > 0);
	CHECK(height > 0);
	CHECK(atlasX + width <= static_cast<GLuint>(font.getAtlasWidth()));
	CHECK(atlasY + height <= static_cast<GLuint>(font.getAtlasHeight()));

	// The space carries no ink, so it only advances.
	CHECK_FALSE(font.getGlyph(' ', atlasX, atlasY, width, height, offsetX, offsetY));
	CHECK(font.getAdvance(' ') > 0.f);

	// Monospaced: a narrow glyph and a wide one advance the same.
	CHECK(font.getAdvance('i') == Catch::Approx(font.getAdvance('W')));
	CHECK(font.getAdvance('1') == Catch::Approx(font.getAdvance('0')));
}

TEST_CASE("The interface font reports a missing metrics file", "[ui][font]")
{
	GLTestContext context;

	Font font;
	CHECK_THROWS(font.load(std::string(ATROPOS_FONT_DIR) + "/does_not_exist.fnt",
		std::string(ATROPOS_FONT_DIR) + "/DejaVuSansMono.tga"));
}