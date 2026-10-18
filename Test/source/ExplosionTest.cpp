#include "game/control/ExplodeOnDeathController.h"
#include "game/control/HealthController.h"
#include "game/control/RemoveOnDeathController.h"
#include "game/GameObject.h"
#include "game/GameWorld.h"
#include "collision/AABBox.h"
#include "collision/Collidable.h"
#include "renderer/render/Explosion.h"
#include "renderer/render/Renderer.h"
#include "renderer/resource/Shader.h"
#include "io/FileLoader.h"
#include "renderer/resource/ShaderSources.h"

#include "NullRenderSystem.h"
#include "NullSoundSystem.h"
#include "GLTestContext.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

TEST_CASE("ExplodeOnDeathController spawns an explosion where the object was", "[explosion][controller]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	ExplodeOnDeathController controller(&soundSystem, "explosion", &renderSystem);

	GameObject object;
	object.setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(1.f, 2.f, 3.f)));
	controller.attach(&object);

	object.setDead();
	controller.update(0.016f);

	REQUIRE(renderSystem.d_explosions.size() == 1);
	CHECK(renderSystem.d_explosions[0] == glm::vec3(1.f, 2.f, 3.f));
	CHECK(soundSystem.d_sfxPlays.size() == 1);
}

TEST_CASE("ExplodeOnDeathController explodes only once", "[explosion][controller]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	ExplodeOnDeathController controller(&soundSystem, "explosion", &renderSystem);

	GameObject object;
	controller.attach(&object);
	object.setDead();

	controller.update(0.016f);
	controller.update(0.016f);
	controller.update(0.016f);

	CHECK(renderSystem.d_explosions.size() == 1);
}

TEST_CASE("ExplodeOnDeathController leaves a living object alone", "[explosion][controller]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;
	ExplodeOnDeathController controller(&soundSystem, "explosion", &renderSystem);

	GameObject object;
	controller.attach(&object);

	controller.update(0.016f);

	CHECK(renderSystem.d_explosions.empty());
}

TEST_CASE("Explosion particles live for their lifetime and then stop drawing", "[explosion][render]")
{
	GLTestContext context;

	Explosion explosion;
	explosion.init(std::string(ATROPOS_DATA_DIR) + "/");

	explosion.addExplosion(glm::vec3(0.f));
	CHECK(explosion.liveParticleCount() > 0);

	explosion.update(0.016f);
	CHECK(explosion.liveParticleCount() > 0);

	explosion.update(5.f);
	CHECK(explosion.liveParticleCount() == 0);

	CHECK(context.sink().errors == 0);
}

TEST_CASE("Explosion covers more of the screen as it expands", "[explosion][render]")
{
	GLTestContext context;

	GLuint texture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGBA8, 64, 64);
	GLuint fbo = 0;
	glCreateFramebuffers(1, &fbo);
	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texture, 0);
	REQUIRE(glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	const glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, -20.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	const glm::mat4 proj = glm::perspective(glm::radians(60.f), 1.f, 0.1f, 100.f);

	// Every particle leaves the spawn point, so a wider spread later means the
	// update advanced them rather than only ageing them.
	std::vector<unsigned char> early(64 * 64 * 4);
	std::vector<unsigned char> late(64 * 64 * 4);

	auto litPixels = [](const std::vector<unsigned char>& pixels)
	{
		int lit = 0;
		for (std::size_t i = 0; i < pixels.size(); i += 4)
		{
			if (pixels[i] + pixels[i + 1] + pixels[i + 2] > 0)
			{
				++lit;
			}
		}
		return lit;
	};

	{
		Explosion explosion;
		explosion.init(std::string(ATROPOS_DATA_DIR) + "/");
		explosion.addExplosion(glm::vec3(0.f));
		explosion.update(0.05f);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, 64, 64);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		explosion.draw(view, proj);
		glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, early.data());
	}

	{
		Explosion explosion;
		explosion.init(std::string(ATROPOS_DATA_DIR) + "/");
		explosion.addExplosion(glm::vec3(0.f));
		explosion.update(0.30f);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, 64, 64);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		explosion.draw(view, proj);
		glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, late.data());
	}

	INFO("early lit=" << litPixels(early) << " late lit=" << litPixels(late));
	CHECK(litPixels(early) > 0);
	CHECK(litPixels(late) > litPixels(early));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK(context.sink().errors == 0);
}

TEST_CASE("Explosion draws bright pixels where it was spawned", "[explosion][render]")
{
	GLTestContext context;

	Explosion explosion;
	explosion.init(std::string(ATROPOS_DATA_DIR) + "/");

	GLuint texture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGBA8, 64, 64);
	GLuint fbo = 0;
	glCreateFramebuffers(1, &fbo);
	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texture, 0);
	REQUIRE(glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Looking down the +Z axis at the origin, so the burst is dead centre.
	const glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, -20.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	const glm::mat4 proj = glm::perspective(glm::radians(60.f), 1.f, 0.1f, 100.f);

	explosion.addExplosion(glm::vec3(0.f));
	// Just after the burst: bright, and still close enough to the origin to cover it.
	explosion.update(0.05f);
	explosion.draw(view, proj);

	CHECK(context.sink().errors == 0);

	std::vector<unsigned char> pixels(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	const std::size_t centre = 32 * 64 * 4 + 32 * 4;
	INFO("centre px=(" << int(pixels[centre]) << "," << int(pixels[centre + 1]) << "," << int(pixels[centre + 2]) << ")");

	// Additive blend onto black, so anything visible means the fireball reached the buffer.
	CHECK(pixels[centre] + pixels[centre + 1] + pixels[centre + 2] > 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TEST_CASE("Explosion draws nothing once every particle has expired", "[explosion][render]")
{
	GLTestContext context;

	Explosion explosion;
	explosion.init(std::string(ATROPOS_DATA_DIR) + "/");

	GLuint texture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGBA8, 64, 64);
	GLuint fbo = 0;
	glCreateFramebuffers(1, &fbo);
	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texture, 0);
	REQUIRE(glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, -20.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	const glm::mat4 proj = glm::perspective(glm::radians(60.f), 1.f, 0.1f, 100.f);

	explosion.addExplosion(glm::vec3(0.f));
	explosion.update(5.f);
	explosion.draw(view, proj);

	CHECK(context.sink().errors == 0);

	std::vector<unsigned char> pixels(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	for (std::size_t i = 0; i < pixels.size(); i += 4)
	{
		CHECK(pixels[i] == 0);
		CHECK(pixels[i + 1] == 0);
		CHECK(pixels[i + 2] == 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TEST_CASE("A destroyed enemy explodes even though the world erases it the same frame", "[explosion][gameobject]")
{
	NullRenderSystem renderSystem;
	NullSoundSystem soundSystem;

	// Must outlive the game object, whose destructor marks the attached one as deleted.
	Collidable collidable(0, AABBox());
	GameWorld world;

	GameObject* enemy = new GameObject;
	enemy->setCollidable(&collidable);

	enemy->addController(std::make_shared<HealthController>(25.f));
	enemy->addController(std::make_shared<ExplodeOnDeathController>(&soundSystem, "explosion", &renderSystem));
	enemy->addController(std::make_shared<RemoveOnDeathController>());

	enemy->setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 5.f, 40.f)));
	world.addObject(enemy);

	collidable.receiveDamage(100.f);
	world.update(0.016f);

	// The explosion has to be placed during this update, because the enemy is gone after it.
	REQUIRE(renderSystem.d_explosions.size() == 1);
	CHECK(renderSystem.d_explosions[0] == glm::vec3(0.f, 5.f, 40.f));
}


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

TEST_CASE("An explosion is drawn past the range where the scene fogs out", "[explosion][render]")
{
	GLTestContext context;

	Shader shadowShader;
	makeShadowShader(shadowShader);

	Renderer renderer(64, 64, &shadowShader, std::string(ATROPOS_DATA_DIR) + "/");
	renderer.setViewMatrix(glm::lookAt(glm::vec3(0.f, 0.f, -20.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)));
	renderer.setProjectionMatrix(glm::perspective(glm::radians(60.f), 1.f, 0.1f, 100.f));
	renderer.setClearColor(glm::vec3(0.f));
	// Fog this tight would swallow any distant geometry. Explosions are gameplay
	// feedback rather than scenery, so they have to survive it.
	renderer.setFog(glm::vec3(0.f), 0.f, 0.1f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.addExplosion(glm::vec3(0.f));
	renderer.startFrame();
	renderer.endFrame(0.05f);

	CHECK(context.sink().errors == 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::vector<unsigned char> pixels(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	const std::size_t centre = 32 * 64 * 4 + 32 * 4;
	INFO("centre px=(" << int(pixels[centre]) << "," << int(pixels[centre + 1]) << "," << int(pixels[centre + 2]) << ")");

	CHECK(pixels[centre] + pixels[centre + 1] + pixels[centre + 2] > 0);
}
