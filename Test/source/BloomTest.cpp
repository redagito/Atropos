#include "renderer/render/Bloom.h"
#include "renderer/render/Explosion.h"
#include "renderer/render/RenderTarget.h"
#include "renderer/render/Renderer.h"
#include "renderer/resource/Shader.h"
#include "io/FileLoader.h"
#include "renderer/resource/ShaderSources.h"

#include "GLTestContext.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace
{
	/** \brief Compiles the shadow shader the renderer needs to be constructed. */
	void makeShadowShader(Shader& shader)
	{
		FileLoader vertexLoader(std::string(ATROPOS_SHADER_SRC_DIR) + "/shadow.vs");
		vertexLoader.load();
		FileLoader fragmentLoader(std::string(ATROPOS_SHADER_SRC_DIR) + "/shadow.fs");
		fragmentLoader.load();

		ShaderSources sources;
		sources.vertex = vertexLoader.getContent();
		sources.fragment = fragmentLoader.getContent();
		shader.init(sources);
	}
}


TEST_CASE("An explosion reaches the screen through the post processing chain", "[explosion][render]")
{
	GLTestContext context;

	Shader shadowShader;
	makeShadowShader(shadowShader);

	Renderer renderer(64, 64, &shadowShader, std::string(ATROPOS_DATA_DIR) + "/");
	renderer.setViewMatrix(glm::lookAt(glm::vec3(0.f, 0.f, -20.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)));
	renderer.setProjectionMatrix(glm::perspective(glm::radians(60.f), 1.f, 0.1f, 100.f));
	// endFrame clears to this, so black makes the fireball the only source of light.
	renderer.setClearColor(glm::vec3(0.f));

	// endFrame presents to the default framebuffer, which is the 64x64 test window.
	// Blanked first, so a lit pixel can only have come from this frame.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.addExplosion(glm::vec3(0.f));
	renderer.startFrame();
	renderer.endFrame(0.05f);

	CHECK(context.sink().errors == 0);

	// Only visible if the offscreen scene survived the trip to the screen. endFrame
	// leaves the scene target bound, so the screen has to be selected again.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::vector<unsigned char> pixels(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	const std::size_t centre = 32 * 64 * 4 + 32 * 4;
	INFO("centre px=(" << int(pixels[centre]) << "," << int(pixels[centre + 1]) << "," << int(pixels[centre + 2]) << ")");

	CHECK(pixels[centre] + pixels[centre + 1] + pixels[centre + 2] > 0);
}

TEST_CASE("Bloom spreads light onto pixels the bright pass did not select", "[explosion][bloom]")
{
	GLTestContext context;

	RenderTarget scene(64, 64, true);
	Bloom bloom;
	bloom.init(std::string(ATROPOS_DATA_DIR) + "/");

	scene.setActive();
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// A block of over bright pixels, standing in for the core of a fireball. Scissoring
	// beats drawing a quad to fill one. A single pixel would not do: a nine tap
	// Gaussian has no energy left two texels out, so the halo would be untestable.
	glScissor(32, 32, 8, 8);
	glEnable(GL_SCISSOR_TEST);
	glClearColor(4.f, 4.f, 4.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	// Read the scene back first, so the same pixel can be compared afterwards.
	std::vector<float> before(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_FLOAT, before.data());

	bloom.apply(scene.getColourTexture(), 64, 64, 1.f);

	// apply() leaves the result on the default framebuffer.
	std::vector<unsigned char> after(64 * 64 * 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, after.data());

	// Past the edge of the bright block, where only the blur can have reached.
	const std::size_t neighbour = 32 * 64 * 4 + 44 * 4;
	INFO("neighbour before=" << before[neighbour] << " after=("
		<< int(after[neighbour]) << "," << int(after[neighbour + 1]) << "," << int(after[neighbour + 2]) << ")");

	CHECK(before[neighbour] == 0.f);
	CHECK(after[neighbour] + after[neighbour + 1] + after[neighbour + 2] > 0);

	CHECK(context.sink().errors == 0);
}
