#include "renderer/render/RenderTarget.h"
#include "renderer/resource/Model.h"
#include "renderer/resource/Mesh.h"
#include "renderer/resource/VertexArrayObject.h"
#include "renderer/resource/VertexBufferObject.h"
#include "renderer/resource/ResourceManager.h"
#include "renderer/resource/AMaterial.h"
#include "core/ThreadPool.h"

#include "GLTestContext.h"

#include <catch2/catch_test_macros.hpp>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace
{
	/** \brief Loads a model and its material through the real resource manager. */
	void loadModel(ResourceManager& manager, const std::string& name)
	{
		manager.loadModel(name);
		for (int i = 0; i < 1000 && !manager.isIdle(); ++i)
		{
			manager.process();
		}
		REQUIRE(manager.hasError() == false);
	}
}

TEST_CASE("The shot is drawn bright enough to bloom", "[shot][render]")
{
	GLTestContext context;

	ThreadPool pool(2);
	ResourceManager manager(std::string(ATROPOS_DATA_DIR) + "/", pool);
	loadModel(manager, "shot");

	const Model* model = manager.getModel("shot");
	REQUIRE(model != nullptr);
	REQUIRE(model->getMaterial() != nullptr);

	RenderTarget target(64, 64, true);
	target.setActive();
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const AMaterial* material = model->getMaterial();
	material->setActive();

	// The shot points along +Z, so put it in front of a camera looking down +Z.
	// It is only a third of a unit across, so the camera has to sit close enough for
	// it to cover whole pixels or the test would measure an empty target.
	const glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -20.f));
	const glm::mat4 rotation = glm::mat4(1.f);
	const glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(3.f));

	material->getShader()->setShaderValueMat4("model", translation * scale * rotation);
	material->getShader()->setShaderValueMat4("rotation", rotation);
	material->getShader()->setShaderValueMat4("translation", translation);
	material->getShader()->setShaderValueMat4("scale", scale);
	material->getShader()->setShaderValueMat4("view", glm::lookAt(glm::vec3(0.f, 0.f, -15.f), glm::vec3(0.f, 0.f, -20.f), glm::vec3(0.f, 1.f, 0.f)));
	material->getShader()->setShaderValueMat4("proj", glm::perspective(glm::radians(60.f), 1.f, 0.1f, 100.f));
	material->getShader()->setShaderValueVec3("fogColor", glm::vec3(0.f));
	material->getShader()->setShaderValueFloat("fogStart", 30.f);
	material->getShader()->setShaderValueFloat("fogEnd", 95.f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, 64, 64);

	model->getVertexArrayObject()->setActive();
	const int vertices = static_cast<int>(model->getMesh()->getVertexBuffer()->getSize() / 3);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	model->getVertexArrayObject()->setInactive();

	CHECK(context.sink().errors == 0);

	// Read the half float target as floats; an 8 bit buffer would clip the answer.
	std::vector<float> pixels(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_FLOAT, pixels.data());

	float brightest = 0.f;
	int lit = 0;
	for (std::size_t i = 0; i < pixels.size(); i += 4)
	{
		const float peak = std::max(pixels[i], std::max(pixels[i + 1], pixels[i + 2]));
		brightest = std::max(brightest, peak);
		if (peak > 0.f)
		{
			++lit;
		}
	}

	INFO("shot peak=" << brightest << " lit pixels=" << lit);

	REQUIRE(lit > 0);

	// One is the bloom bright pass threshold, and the sky the shot flies through is
	// below it, so anything above this both glows and stays clear of the background.
	CHECK(brightest > 1.f);
}