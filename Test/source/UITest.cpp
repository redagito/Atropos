#include "GLTestContext.h"
#include "renderer/ui/Font.h"
#include "renderer/ui/UI.h"
#include "game/HUD.h"
#include "renderer/render/Renderer.h"
#include "renderer/resource/Shader.h"
#include "io/FileLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

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
namespace
{
	/** \brief Offscreen colour target the UI draws into. */
	struct UICanvas
	{
		GLuint texture = 0;
		GLuint framebuffer = 0;
		int size = 0;

		UICanvas(int pixels)
			:
			size(pixels)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &texture);
			glTextureStorage2D(texture, 1, GL_RGBA8, size, size);
			glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glCreateFramebuffers(1, &framebuffer);
			glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture, 0);
			REQUIRE(glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}

		~UICanvas()
		{
			glDeleteFramebuffers(1, &framebuffer);
			glDeleteTextures(1, &texture);
		}

		std::vector<unsigned char> read() const
		{
			std::vector<unsigned char> pixels(static_cast<std::size_t>(size) * size * 4);
			glReadPixels(0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
			return pixels;
		}
	};

	/** \brief glReadPixels returns rows bottom up, so convert from a top origin row. */
	int flipped(int topRow, int size)
	{
		return size - 1 - topRow;
	}

	bool isRed(const std::vector<unsigned char>& pixels, int x, int y, int size)
	{
		// glReadPixels returns rows bottom up.
		const std::size_t index = (static_cast<std::size_t>(y) * size + x) * 4;
		return pixels[index] > 128 && pixels[index + 1] < 80 && pixels[index + 2] < 80;
	}
}

TEST_CASE("A user interface rectangle fills the pixels it covers", "[ui][render]")
{
	GLTestContext context;

	UI ui;
	ui.init(std::string(ATROPOS_DATA_DIR) + "/");
	ui.onResize(64, 64);

	UICanvas canvas(64);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.framebuffer);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	ui.beginFrame();
	ui.drawRect(glm::vec2(16.f, 40.f), glm::vec2(32.f, 8.f), glm::vec4(1.f, 0.f, 0.f, 1.f));
	ui.flush();

	CHECK(context.sink().errors == 0);

	const std::vector<unsigned char> pixels = canvas.read();

// The rectangle covers x 16..48 and y 40..48 in top origin coordinates.
	CHECK(isRed(pixels, 32, flipped(44, 64), 64));
	CHECK(isRed(pixels, 20, flipped(47, 64), 64));
	// Outside it, on all four sides.
	CHECK_FALSE(isRed(pixels, 8, flipped(44, 64), 64));
	CHECK_FALSE(isRed(pixels, 56, flipped(44, 64), 64));
	CHECK_FALSE(isRed(pixels, 32, flipped(30, 64), 64));
	CHECK_FALSE(isRed(pixels, 32, flipped(55, 64), 64));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TEST_CASE("User interface text puts lighter pixels on the background", "[ui][render]")
{
	GLTestContext context;

	UI ui;
	ui.init(std::string(ATROPOS_DATA_DIR) + "/");
	ui.onResize(320, 64);

	UICanvas canvas(320);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.framebuffer);
	glViewport(0, 0, 320, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	ui.beginFrame();
	ui.drawText(glm::vec2(8.f, 12.f), "SCORE 87", 1.f, glm::vec4(1.f, 1.f, 1.f, 1.f));
	ui.flush();

	CHECK(context.sink().errors == 0);

	const std::vector<unsigned char> pixels = canvas.read();

	std::size_t lit = 0;
	for (std::size_t i = 0; i < pixels.size(); i += 4)
	{
		if (pixels[i] > 128)
		{
			lit++;
		}
	}

	INFO("glyph pixels: " << lit);
	CHECK(lit > 200);

	// The advance of eight glyphs must move the pen by the summed advances.
	CHECK(ui.measureText("SCORE 87", 1.f) == Catch::Approx(8.f * ui.measureText("S", 1.f)));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


namespace
{
	/** \brief Reads one pixel as colour components. */
	void pixelAt(const std::vector<unsigned char>& pixels, int x, int y, int size,
		int& red, int& green, int& blue)
	{
		const std::size_t index = (static_cast<std::size_t>(y) * size + x) * 4;
		red = pixels[index];
		green = pixels[index + 1];
		blue = pixels[index + 2];
	}
}

TEST_CASE("The progress bar draws a fill over a darker track", "[ui][hud]")
{
	GLTestContext context;

	UI ui;
	ui.init(std::string(ATROPOS_DATA_DIR) + "/");
	ui.onResize(200, 100);

	UICanvas canvas(200);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.framebuffer);
	glViewport(0, 0, 200, 100);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

ui.beginFrame();
	// Two fifths of 200 is 80 wide, centred, so the bar spans x 60..140 at y 70..84.
	HUD::drawBar(ui, 70.f, 14.f, 0.5f, glm::vec4(1.f, 0.f, 0.f, 1.f));
	ui.flush();

	CHECK(context.sink().errors == 0);

	const std::vector<unsigned char> pixels = canvas.read();

	int red = 0;
	int green = 0;
	int blue = 0;

	// Left half of the bar, 60..100, is the red fill.
	pixelAt(pixels, 80, flipped(76, 100), 200, red, green, blue);
	CHECK(red > 128);
	CHECK(green < 80);
	CHECK(blue < 80);

	// Right half, 100..140, is the unfilled track: dark, but not the black background.
	pixelAt(pixels, 120, flipped(76, 100), 200, red, green, blue);
	CHECK(red < 80);
	CHECK(red > 10);

	// Nothing is drawn to the left of the bar.
	pixelAt(pixels, 10, flipped(76, 100), 200, red, green, blue);
	CHECK(red == 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TEST_CASE("The frame rate readout tracks the frame time it is given", "[ui][hud]")
{
	GLTestContext context;

	UI ui;
	ui.init(std::string(ATROPOS_DATA_DIR) + "/");

	// A single frame seeds the value exactly, so no smoothing to fight with.
	ui.setFrameTime(1.f / 60.f);
	CHECK(ui.getFps() == 60);

	// Repeated frames at the same rate stay put.
	for (int i = 0; i < 30; ++i)
	{
		ui.setFrameTime(1.f / 60.f);
	}
	CHECK(ui.getFps() == 60);

	// A zero or negative delta must not divide by zero.
	ui.setFrameTime(0.f);
	ui.setFrameTime(-1.f);
	CHECK(ui.getFps() == 60);
}

TEST_CASE("Interface queued after the scene draw reaches the next frame", "[ui][frame]")
{
	GLTestContext context;

	FileLoader vertexLoader(std::string(ATROPOS_SHADER_SRC_DIR) + "/shadow.vs");
	vertexLoader.load();
	FileLoader fragmentLoader(std::string(ATROPOS_SHADER_SRC_DIR) + "/shadow.fs");
	fragmentLoader.load();

	ShaderSources shadowSources;
	shadowSources.vertex = vertexLoader.getContent();
	shadowSources.fragment = fragmentLoader.getContent();

	Shader shadowShader;
	shadowShader.init(shadowSources);

	Renderer renderer(64, 64, &shadowShader, std::string(ATROPOS_DATA_DIR) + "/");

	UICanvas canvas(64);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.framebuffer);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// The engine draws the scene first and runs the game state afterwards, so
	// anything the state queues lands after the flush and must survive to the
	// next frame rather than being cleared at the start of one.
	renderer.startFrame();
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.endFrame();

	renderer.ui().drawRect(glm::vec2(16.f, 16.f), glm::vec2(32.f, 16.f), glm::vec4(1.f, 0.f, 0.f, 1.f));

	renderer.startFrame();
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.endFrame();

	CHECK(context.sink().errors == 0);

	const std::vector<unsigned char> pixels = canvas.read();
	CHECK(isRed(pixels, 32, flipped(24, 64), 64));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
TEST_CASE("A label is drawn over a translucent panel", "[ui][hud]")
{
	GLTestContext context;

	UI ui;
	ui.init(std::string(ATROPOS_DATA_DIR) + "/");
	ui.onResize(200, 100);

	UICanvas canvas(200);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.framebuffer);
	glViewport(0, 0, 200, 100);
	// A bright scene, which is what the panel has to stay legible over.
	glClearColor(0.9f, 0.9f, 0.2f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	ui.beginFrame();
	HUD::drawLabel(ui, glm::vec2(20.f, 20.f), "87%", 1.f, glm::vec4(1.f, 1.f, 1.f, 1.f));
	ui.flush();

	CHECK(context.sink().errors == 0);

	const std::vector<unsigned char> pixels = canvas.read();

	int red = 0;
	int green = 0;
	int blue = 0;

	// Just inside the panel padding, above the glyphs, the background is darkened
	// but not replaced, because the panel is translucent.
	pixelAt(pixels, 20, flipped(22, 100), 200, red, green, blue);
	CHECK(red < 200);
	CHECK(red > 100);
	CHECK(green < 200);

	// Well clear of the label the background is untouched.
	pixelAt(pixels, 180, flipped(22, 100), 200, red, green, blue);
	CHECK(red > 200);
	CHECK(green > 200);

	// The glyphs themselves are on top of the panel, so bright white survives.
	std::size_t white = 0;
	for (std::size_t i = 0; i < pixels.size(); i += 4)
	{
		if (pixels[i] > 240 && pixels[i + 1] > 240 && pixels[i + 2] > 240)
		{
			white++;
		}
	}
	INFO("glyph pixels on the panel: " << white);
	CHECK(white > 20);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

