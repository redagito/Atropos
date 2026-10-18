#include "renderer/resource/ResourceManager.h"

#include "io/ObjModelLoader.h"
#include "io/ConfigFile.h"
#include "io/FileLoader.h"

#include "renderer/resource/BasicMaterial.h"
#include "renderer/resource/ColorMaterial.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

ResourceManager::ResourceManager(const std::string& resourceBasePath, ThreadPool& pool)
	:
	d_resourcePath(resourceBasePath),
	d_shaders(pool),
	d_textures(pool),
	d_meshes(pool),
	d_materials(pool),
	d_models(pool)
{
}

ResourceManager::~ResourceManager()
{
}

ShaderSources ResourceManager::parseShader(const std::string& name) const
{
	const std::string shaderFile = d_resourcePath + "shader/" + name + ".sha";

	ConfigFile shaderConfig;
	shaderConfig.load(shaderFile);

	if (!shaderConfig.has("vertexshader"))
	{
		throw std::runtime_error("Missing vertexshader property in shader definition file: " + shaderFile);
	}
	if (!shaderConfig.has("fragmentshader"))
	{
		throw std::runtime_error("Missing fragmentshader property in shader definition file: " + shaderFile);
	}

	const std::string vertexShader(d_resourcePath + "shader/src/" + shaderConfig.get("vertexshader", "error"));
	const std::string fragmentShader(d_resourcePath + "shader/src/" + shaderConfig.get("fragmentshader", "error"));

	FileLoader vertexLoader(vertexShader);
	vertexLoader.load();
	FileLoader fragmentLoader(fragmentShader);
	fragmentLoader.load();

	ShaderSources sources;
	sources.vertex = vertexLoader.getContent();
	sources.fragment = fragmentLoader.getContent();
	return sources;
}

MeshData ResourceManager::parseMesh(const std::string& name) const
{
	const std::string meshFile = d_resourcePath + "mesh/" + name + ".obj";
	SPDLOG_DEBUG("Parsing mesh {} from file {}.", name, meshFile);

	ObjModelLoader loader;
	loader.load(meshFile);

	MeshData data;
	data.vertices = loader.getVertices();
	data.normals = loader.getNormals();
	data.uv = loader.getUV();
	data.box = loader.getAABBox();
	return data;
}

Image ResourceManager::parseTexture(const std::string& name) const
{
	const std::string textureFile = d_resourcePath + "texture/" + name + ".tga";
	SPDLOG_DEBUG("Parsing texture {}.", textureFile);

	Image image;
	image.load(textureFile);
	return image;
}

MaterialDesc ResourceManager::parseMaterial(const std::string& name) const
{
	const std::string materialFile = d_resourcePath + "material/" + name + ".mat";
	ConfigFile conf;
	conf.load(materialFile);

	if (!conf.has("class"))
	{
		throw std::runtime_error("Missing class property in material file: " + materialFile);
	}
	if (!conf.has("shader"))
	{
		throw std::runtime_error("Missing shader property in material file: " + materialFile);
	}

	MaterialDesc desc;
	desc.materialClass = conf.get("class", "error");
	desc.shader = conf.get("shader", "error");

	if (desc.materialClass == "basic")
	{
		if (!conf.has("texture0"))
		{
			throw std::runtime_error("Missing texture0 property in material file: " + materialFile);
		}
		if (!conf.has("ambient"))
		{
			SPDLOG_WARN("Missing ambient property in material file {}.", materialFile);
		}
		if (!conf.has("specular"))
		{
			SPDLOG_WARN("Missing specular property in material file {}.", materialFile);
		}
		desc.texture = conf.get("texture0", "error");
		desc.ambient = conf.get("ambient", 0.1f);
		desc.specular = conf.get("specular", 0.2f);
	}
	else if (desc.materialClass == "color")
	{
		if (!conf.has("color"))
		{
			throw std::runtime_error("Missing color property in material file: " + materialFile);
		}
		desc.color = conf.get("color", glm::vec3(1.f, 0.f, 0.f));
	}
	else
	{
		throw std::runtime_error("Unknown class name property value in material file: " + materialFile);
	}

	return desc;
}

ModelDesc ResourceManager::parseModel(const std::string& name) const
{
	const std::string modelFile(d_resourcePath + "model/" + name + ".mdl");
	ConfigFile conf;
	conf.load(modelFile);

	if (!conf.has("mesh"))
	{
		throw std::runtime_error("Missing mesh parameter in model file: " + modelFile);
	}
	if (!conf.has("material"))
	{
		throw std::runtime_error("Missing material parameter in model file: " + modelFile);
	}

	ModelDesc desc;
	desc.mesh = conf.get("mesh", "error");
	desc.material = conf.get("material", "error");
	return desc;
}

void ResourceManager::loadShader(const std::string& name)
{
	d_shaders.request(name,
		[this, name] { return parseShader(name); },
		[](const ShaderSources& sources)
		{
			auto shader = std::make_unique<Shader>();
			shader->init(sources);
			return shader;
		});
}

void ResourceManager::loadTexture(const std::string& name)
{
	d_textures.request(name,
		[this, name] { return parseTexture(name); },
		[](const Image& image)
		{
			auto texture = std::make_unique<Texture>();
			texture->init(image);
			return texture;
		});
}

void ResourceManager::loadMesh(const std::string& name)
{
	d_meshes.request(name,
		[this, name] { return parseMesh(name); },
		[](const MeshData& data)
		{
			auto mesh = std::make_unique<Mesh>();
			mesh->init(data);
			return mesh;
		});
}

void ResourceManager::loadMaterial(const std::string& name)
{
	d_materials.request(name,
		[this, name] { return parseMaterial(name); },
		[this](const MaterialDesc& desc) -> std::unique_ptr<AMaterial>
		{
			Shader* shader = lookupShader(desc.shader);
			if (desc.materialClass == "basic")
			{
				Texture* texture = lookupTexture(desc.texture);
				return std::make_unique<BasicMaterial>(shader, desc.ambient, desc.specular, texture);
			}
			return std::make_unique<ColorMaterial>(shader, desc.color);
		},
		{},
		[this](const MaterialDesc& desc)
		{
			std::vector<std::shared_ptr<ResourceSlotBase>> deps;
			loadShader(desc.shader);
			deps.push_back(d_shaders.getHandle(desc.shader).getSlot());
			if (desc.materialClass == "basic")
			{
				loadTexture(desc.texture);
				deps.push_back(d_textures.getHandle(desc.texture).getSlot());
			}
			return deps;
		});
}

void ResourceManager::loadModel(const std::string& name)
{
	d_models.request(name,
		[this, name] { return parseModel(name); },
		[this](const ModelDesc& desc) -> std::unique_ptr<Model>
		{
			Mesh* mesh = lookupMesh(desc.mesh);
			AMaterial* material = lookupMaterial(desc.material);
			return std::make_unique<Model>(mesh, material);
		},
		{},
		[this](const ModelDesc& desc)
		{
			std::vector<std::shared_ptr<ResourceSlotBase>> deps;
			loadMesh(desc.mesh);
			deps.push_back(d_meshes.getHandle(desc.mesh).getSlot());
			loadMaterial(desc.material);
			deps.push_back(d_materials.getHandle(desc.material).getSlot());
			return deps;
		});
}

Shader* ResourceManager::lookupShader(const std::string& name) const
{
	return d_shaders.getHandle(name).get();
}

Texture* ResourceManager::lookupTexture(const std::string& name) const
{
	return d_textures.getHandle(name).get();
}

Mesh* ResourceManager::lookupMesh(const std::string& name) const
{
	return d_meshes.getHandle(name).get();
}

AMaterial* ResourceManager::lookupMaterial(const std::string& name) const
{
	return d_materials.getHandle(name).get();
}

void ResourceManager::process()
{
	// Dependencies are committed before their dependents.
	d_shaders.process();
	d_textures.process();
	d_meshes.process();
	d_materials.process();
	d_models.process();
}

bool ResourceManager::isIdle() const
{
	return d_shaders.isIdle()
		&& d_textures.isIdle()
		&& d_meshes.isIdle()
		&& d_materials.isIdle()
		&& d_models.isIdle();
}

bool ResourceManager::hasError() const
{
	return d_shaders.hasError()
		|| d_textures.hasError()
		|| d_meshes.hasError()
		|| d_materials.hasError()
		|| d_models.hasError();
}

template <typename T, typename Parsed>
T* ResourceManager::waitFor(AsyncLoader<T, Parsed>& loader, const std::string& name)
{
	ResourceHandle<T> handle = loader.getHandle(name);
	while (!handle.isReady() && !handle.isFailed())
	{
		process();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return handle.get();
}

Shader* ResourceManager::getShader(const std::string& name)
{
	loadShader(name);
	return waitFor(d_shaders, name);
}

Texture* ResourceManager::getTexture(const std::string& name)
{
	loadTexture(name);
	return waitFor(d_textures, name);
}

Mesh* ResourceManager::getMesh(const std::string& name)
{
	loadMesh(name);
	return waitFor(d_meshes, name);
}

AMaterial* ResourceManager::getMaterial(const std::string& name)
{
	loadMaterial(name);
	return waitFor(d_materials, name);
}

Model* ResourceManager::getModel(const std::string& name)
{
	loadModel(name);
	return waitFor(d_models, name);
}
