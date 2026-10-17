#include "renderer/resource/ResourceManager.h"

#include "io/ObjModelLoader.h"
#include "io/ConfigFile.h"

#include "renderer/resource/BasicMaterial.h"
#include "renderer/resource/ColorMaterial.h"

#include "resource/Image.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <stdexcept>
#include <string>

ResourceManager::ResourceManager(const std::string& resourceBasePath)
	:
	d_resourcePath(resourceBasePath)
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::loadModel(const std::string& name)
{
	if (d_modelMap.count(name) != 0)
	{
		return;
	}

	SPDLOG_DEBUG("Loading model: {}.", name);
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

	Mesh* mesh = getMesh(conf.get("mesh", "error"));
	AMaterial* material = getMaterial(conf.get("material", "error"));
	d_modelMap[name].reset(new Model(mesh, material));
}

void ResourceManager::loadMesh(const std::string& name)
{
	if (d_meshMap.count(name) != 0)
	{
		return;
	}

	const std::string meshFile = d_resourcePath + "mesh/" + name + ".obj";
	SPDLOG_DEBUG("Creating new mesh {} from file {}.", name, meshFile);

	ObjModelLoader loader;
	loader.load(meshFile);

	std::unique_ptr<Mesh> mesh(new Mesh);
	mesh->init(loader.getVertices(), loader.getNormals(), loader.getUV(), loader.getAABBox());
	d_meshMap[name] = std::move(mesh);
}

void ResourceManager::loadMaterial(const std::string& name)
{
	if (d_materialMap.count(name) != 0)
	{
		return;
	}

	SPDLOG_DEBUG("Loading new material {}.", name);
	ConfigFile conf;
	const std::string materialFile = d_resourcePath + "material/" + name + ".mat";
	conf.load(materialFile);

	if (!conf.has("class"))
	{
		throw std::runtime_error("Missing class property in material file: " + materialFile);
	}
	if (!conf.has("shader"))
	{
		throw std::runtime_error("Missing shader property in material file: " + materialFile);
	}

	const std::string materialClass = conf.get("class", "error");
	const std::string shader = conf.get("shader", "error");

	std::unique_ptr<AMaterial> material(nullptr);
	if (materialClass == "basic")
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

		const std::string texture = conf.get("texture0", "error");
		material.reset(new BasicMaterial(getShader(shader), conf.get("ambient", 0.1f), conf.get("specular", 0.2f), getTexture(texture)));
	}
	else if (materialClass == "color")
	{
		if (!conf.has("color"))
		{
			throw std::runtime_error("Missing color property in material file: " + materialFile);
		}
		const glm::vec3 color = conf.get("color", glm::vec3(1.f, 0.f, 0.f));
		material.reset(new ColorMaterial(getShader(shader), color));
	}
	else
	{
		throw std::runtime_error("Unknown class name property value in material file: " + materialFile);
	}

	d_materialMap[name] = std::move(material);
}

void ResourceManager::loadTexture(const std::string& name)
{
	if (d_textureMap.count(name) != 0)
	{
		return;
	}

	Image image;
	const std::string textureFile = d_resourcePath + "texture/" + name + ".tga";
	SPDLOG_DEBUG("Loading texture {} from file.", textureFile);
	image.load(textureFile);

	std::unique_ptr<Texture> texture(new Texture);
	texture->init(image);
	d_textureMap[name] = std::move(texture);
}

void ResourceManager::loadShader(const std::string& name)
{
	if (d_shaderMap.count(name) != 0)
	{
		return;
	}

	// Shader definition files are located in the shader sub directory of the resource path
	const std::string shaderFile = d_resourcePath + "shader/" + name + ".sha";
	SPDLOG_DEBUG("Loading shader {} from file {}.", name, shaderFile);

	// Load shader config file with names of shader sources
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

	// Create path to the sources.
	// Shader source files are located at the src/ directory in the shader path
	const std::string vertexShader(d_resourcePath + "shader/src/" + shaderConfig.get("vertexshader", "error"));
	const std::string fragmentShader(d_resourcePath + "shader/src/" + shaderConfig.get("fragmentshader", "error"));

	std::unique_ptr<Shader> shader(new Shader);
	shader->init(vertexShader, fragmentShader);
	d_shaderMap[name] = std::move(shader);
}

Shader* ResourceManager::getShader(const std::string& name)
{
	if (d_shaderMap.count(name) == 0)
	{
		loadShader(name);
	}
	return d_shaderMap.at(name).get();
}

Texture* ResourceManager::getTexture(const std::string& name)
{
	if (d_textureMap.count(name) == 0)
	{
		loadTexture(name);
	}
	return d_textureMap.at(name).get();
}

Mesh* ResourceManager::getMesh(const std::string& meshName)
{
	if (d_meshMap.count(meshName) == 0)
	{
		loadMesh(meshName);
	}
	return d_meshMap.at(meshName).get();
}

AMaterial* ResourceManager::getMaterial(const std::string& name)
{
	if (d_materialMap.count(name) == 0)
	{
		loadMaterial(name);
	}
	return d_materialMap.at(name).get();
}

Model* ResourceManager::getModel(const std::string& name)
{
	if (d_modelMap.count(name) == 0)
	{
		loadModel(name);
	}
	return d_modelMap.at(name).get();
}
