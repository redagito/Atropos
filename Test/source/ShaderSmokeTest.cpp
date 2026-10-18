#include <glad/gl.h>

#include "GLTestContext.h"
#include "io/FileLoader.h"
#include "renderer/resource/ColorMaterial.h"
#include "renderer/resource/Mesh.h"
#include "renderer/resource/MeshData.h"
#include "renderer/resource/Model.h"
#include "renderer/resource/Shader.h"
#include "renderer/resource/Texture.h"
#include "renderer/render/ShadowBuffer.h"
#include "resource/Image.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <utility>
#include <vector>

namespace
{
	using GLContext = GLTestContext;
	std::string loadStage(const std::string& name)
	{
		FileLoader loader(std::string(ATROPOS_SHADER_SRC_DIR) + "/" + name);
		loader.load();
		return loader.getContent();
	}

	std::string shaderLog(GLuint stage)
	{
		GLint length = 0;
		glGetShaderiv(stage, GL_INFO_LOG_LENGTH, &length);
		if (length <= 1)
		{
			return "";
		}
		std::vector<char> log(static_cast<std::size_t>(length));
		glGetShaderInfoLog(stage, length, nullptr, log.data());
		return std::string(log.data());
	}

	std::string programLog(GLuint program)
	{
		GLint length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		if (length <= 1)
		{
			return "";
		}
		std::vector<char> log(static_cast<std::size_t>(length));
		glGetProgramInfoLog(program, length, nullptr, log.data());
		return std::string(log.data());
	}

	GLuint compileStage(const std::string& name, const std::string& source, GLenum type)
	{
		const GLuint stage = glCreateShader(type);
		const char* text = source.c_str();
		glShaderSource(stage, 1, &text, nullptr);
		glCompileShader(stage);

		GLint compiled = GL_FALSE;
		glGetShaderiv(stage, GL_COMPILE_STATUS, &compiled);
		INFO("compile log " << name << ":\n" << shaderLog(stage));
		CHECK(compiled == GL_TRUE);
		return stage;
	}
}

TEST_CASE("Shader programs compile and link under the core profile", "[shader]")
{
	GLContext context;

	GLint major = 0;
	GLint minor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	INFO("GL_VERSION: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	CHECK(major == 4);
	CHECK(minor >= 6);
	CHECK(GLAD_VERSION_MAJOR(context.version()) == major);
	CHECK(GLAD_VERSION_MINOR(context.version()) == minor);

	const std::vector<std::pair<std::string, std::pair<std::string, std::string>>> programs =
	{
		{"color", {"color.vs", "color.fs"}},
		{"shadow", {"shadow.vs", "shadow.fs"}},
		{"texturedPhong", {"texturedPhong.vs", "texturedPhong.fs"}},
		{"ui", {"ui.vs", "ui.fs"}},
	};

	for (const auto& program : programs)
	{
		const GLuint vertex = compileStage(program.first + "/" + program.second.first, loadStage(program.second.first), GL_VERTEX_SHADER);
		const GLuint fragment = compileStage(program.first + "/" + program.second.second, loadStage(program.second.second), GL_FRAGMENT_SHADER);

		const GLuint handle = glCreateProgram();
		glAttachShader(handle, vertex);
		glAttachShader(handle, fragment);
		glLinkProgram(handle);

		GLint linked = GL_FALSE;
		glGetProgramiv(handle, GL_LINK_STATUS, &linked);
		INFO("link log " << program.first << ":\n" << programLog(handle));
		CHECK(linked == GL_TRUE);

		if (linked == GL_TRUE && program.first != "ui")
		{
			// Every shader uses vertex; the unused ones lose normal/uv to dead code
			// elimination and report -1, which is why the mesh setup skips them.
			CHECK(glGetAttribLocation(handle, "vertex") == 0);

			const GLint normal = glGetAttribLocation(handle, "normal");
			INFO("normal location: " << normal);
			CHECK((normal == -1 || normal == 1));

			const GLint uv = glGetAttribLocation(handle, "uv");
			INFO("uv location: " << uv);
			CHECK((uv == -1 || uv == 2));
		}

		glDeleteProgram(handle);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	INFO("GL errors while compiling and linking: " << context.sink().errors);
	CHECK(context.sink().errors == 0);
}

TEST_CASE("Textures allocate their whole mip chain through direct state access", "[shader][texture]")
{
	GLContext context;

	Image image;
	image.load(std::string(ATROPOS_TEXTURE_DIR) + "/checkerPattern.tga");

	Texture texture;
	texture.init(image);
	texture.setActive(0);

	CHECK(context.sink().errors == 0);

	GLint bound = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
	REQUIRE(bound != 0);

	GLint levelWidth = 0;
	GLint levelHeight = 0;
	glGetTextureLevelParameteriv(static_cast<GLuint>(bound), 0, GL_TEXTURE_WIDTH, &levelWidth);
	glGetTextureLevelParameteriv(static_cast<GLuint>(bound), 0, GL_TEXTURE_HEIGHT, &levelHeight);
	CHECK(levelWidth == static_cast<GLint>(image.getWidth()));
	CHECK(levelHeight == static_cast<GLint>(image.getHeight()));

	// A 128x128 chain is log2(128)+1 = 8 levels, so level 7 is the 1x1 smallest one.
	GLint smallest = 0;
	glGetTextureLevelParameteriv(static_cast<GLuint>(bound), 7, GL_TEXTURE_WIDTH, &smallest);
	CHECK(smallest == 1);

	GLint pastEnd = 0;
	glGetTextureLevelParameteriv(static_cast<GLuint>(bound), 8, GL_TEXTURE_WIDTH, &pastEnd);
	CHECK(pastEnd == 0);

	GLint immutable = 0;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_IMMUTABLE_FORMAT, &immutable);
	CHECK(immutable == GL_TRUE);
}

namespace
{
	/** \brief Reads back what an attribute points at; the classic queries read the bound VAO. */
	void attributeSource(GLuint location, GLint& buffer, GLint& components)
	{
		GLint enabled = 0;
		glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		REQUIRE(enabled == GL_TRUE);

		glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_SIZE, &components);
		glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
	}
}

TEST_CASE("Models set up their vertex array through direct state access", "[shader][model]")
{
	GLContext context;

	ShaderSources sources;
	sources.vertex = loadStage("texturedPhong.vs");
	sources.fragment = loadStage("texturedPhong.fs");

	Shader shader;
	shader.init(sources);

	MeshData data;
	data.vertices = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f};
	data.normals = {0.f, 0.f, 1.f, 0.f, 0.f, 1.f};
	data.uv = {0.f, 0.f, 1.f, 0.f};

	Mesh mesh;
	mesh.init(data);

	ColorMaterial material(&shader, glm::vec3(1.f));
	Model model(&mesh, &material);

	CHECK(context.sink().errors == 0);

	// Uniforms come from the link time enumeration, so the names the renderer sets must resolve.
	CHECK(shader.getUniformLocation("model") != -1);
	CHECK(shader.getUniformLocation("view") != -1);
	CHECK(shader.getUniformLocation("bias") != -1);
	CHECK(shader.getUniformLocation("notAUniform") == -1);

	const GLuint vao = model.getVertexArrayObject()->getId();
	glBindVertexArray(vao);

	GLint vertexBuffer = 0;
	GLint vertexComponents = 0;
	attributeSource(0, vertexBuffer, vertexComponents);
	CHECK(vertexComponents == 3);
	CHECK(vertexBuffer == static_cast<GLint>(mesh.getVertexBuffer()->getId()));

	GLint normalBuffer = 0;
	GLint normalComponents = 0;
	attributeSource(1, normalBuffer, normalComponents);
	CHECK(normalComponents == 3);
	CHECK(normalBuffer == static_cast<GLint>(mesh.getNormalBuffer()->getId()));

	GLint uvBuffer = 0;
	GLint uvComponents = 0;
	attributeSource(2, uvBuffer, uvComponents);
	CHECK(uvComponents == 2);
	CHECK(uvBuffer == static_cast<GLint>(mesh.getUVBuffer()->getId()));

	// Separate buffers mean separate slots, otherwise the last bind would win for all three.
	CHECK(vertexBuffer != normalBuffer);
	CHECK(normalBuffer != uvBuffer);
	CHECK(vertexBuffer != uvBuffer);

	glBindVertexArray(0);

	GLint size = 0;
	glGetNamedBufferParameteriv(vertexBuffer, GL_BUFFER_SIZE, &size);
	CHECK(size == static_cast<GLint>(data.vertices.size() * sizeof(float)));
}

TEST_CASE("A model draws coloured pixels through its vertex array", "[shader][render]")
{
	GLContext context;

	ShaderSources sources;
	sources.vertex = loadStage("color.vs");
	sources.fragment = loadStage("color.fs");

	Shader shader;
	shader.init(sources);

	MeshData data;
	data.vertices = {-0.8f, -0.8f, 0.f, 0.8f, -0.8f, 0.f, 0.f, 0.8f, 0.f};
	data.normals = {0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f};
	data.uv = {0.f, 0.f, 1.f, 0.f, 0.5f, 1.f};

	Mesh mesh;
	mesh.init(data);

	ColorMaterial material(&shader, glm::vec3(0.f, 1.f, 0.f));
	Model model(&mesh, &material);

	GLuint color = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &color);
	glTextureStorage2D(color, 1, GL_RGBA8, 64, 64);
	glTextureParameteri(color, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(color, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLuint target = 0;
	glCreateFramebuffers(1, &target);
	glNamedFramebufferTexture(target, GL_COLOR_ATTACHMENT0, color, 0);
	REQUIRE(glCheckNamedFramebufferStatus(target, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, target);
	glViewport(0, 0, 64, 64);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	material.setActive();
	glm::mat4 identity(1.f);
	shader.setShaderValueMat4("rotation", identity);
	shader.setShaderValueMat4("translation", identity);
	shader.setShaderValueMat4("scale", identity);
	shader.setShaderValueMat4("view", identity);
	shader.setShaderValueMat4("proj", identity);

glBindVertexArray(model.getVertexArrayObject()->getId());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	CHECK(context.sink().errors == 0);

	std::vector<unsigned char> pixels(64 * 64 * 4);
	glReadPixels(0, 0, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	std::size_t coloured = 0;
	for (std::size_t i = 0; i < pixels.size(); i += 4)
	{
		if (pixels[i + 1] > 128)
		{
			coloured++;
		}
	}

	INFO("green pixels: " << coloured);
	CHECK(coloured > 100);

glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &target);
	glDeleteTextures(1, &color);
}

TEST_CASE("The shadow framebuffer is complete through named state access", "[shader][shadow]")
{
	GLContext context;

	ShadowBuffer shadowBuffer;

	CHECK(context.sink().errors == 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLint status = 0;
	glGetNamedFramebufferAttachmentParameteriv(shadowBuffer.getFramebufferId(), GL_DEPTH_ATTACHMENT,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &status);
	CHECK(status == GL_TEXTURE);

	GLint textureName = 0;
	glGetNamedFramebufferAttachmentParameteriv(shadowBuffer.getFramebufferId(), GL_DEPTH_ATTACHMENT,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &textureName);
	CHECK(static_cast<GLuint>(textureName) == shadowBuffer.getDepthTexture());

	// A depth-only target has no colour attachment, which is what glDrawBuffer(GL_NONE) means.
	shadowBuffer.setActive();

	GLint drawBuffer = 0;
	glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);
	CHECK(drawBuffer == GL_NONE);

	GLint readBuffer = 0;
	glGetIntegerv(GL_READ_BUFFER, &readBuffer);
	CHECK(readBuffer == GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK(context.sink().errors == 0);
}