#include "renderer/resource/ResourceManager.h"

#include "io/ObjModelLoader.h"
#include "io/ConfigFile.h"

#include "renderer/resource/BasicMaterial.h"
#include "renderer/resource/ColorMaterial.h"

#include "resource/Image.h"
#include <spdlog/spdlog.h>



ResourceManager::ResourceManager(const std::string& resourceBasePath)
	:
	d_resourcePath(resourceBasePath)
{
	
}

ResourceManager::~ResourceManager()
{

}

bool ResourceManager::loadModel(const std::string& name)
{
	if (d_modelMap.count(name) == 0)
	{
		SPDLOG_DEBUG("Loading model: {}.", name.c_str());
		// Model file name
		std::string modelFile(d_resourcePath + "model/" + name + ".mdl");
		// Model config
		ConfigFile conf;
		// Load and error checking
		if (!conf.load(((d_resourcePath + "model/" + name + ".mdl"))))
		{
			SPDLOG_ERROR("Failed to load model {} from file {}.", name.c_str(), modelFile.c_str());
			return false;
		}
		// Load into map
		if (!conf.has("mesh"))
		{
			SPDLOG_ERROR("Missing mesh parameter in model file {}.", modelFile.c_str());
			return false;
		}
		if (!conf.has("material"))
		{
			SPDLOG_ERROR("Missing material parameter in model file {}.", modelFile.c_str());
			return false;
		}
		if (!loadMesh(conf.get("mesh", "error")))
		{
			return false;
		}

		Mesh* mesh = getMesh(conf.get("mesh", "error"));
		AMaterial* material = getMaterial(conf.get("material", "error"));
		if (mesh == nullptr || material == nullptr)
		{
			SPDLOG_ERROR("Failed to load model {}: missing mesh or material.", name.c_str());
			return false;
		}

		d_modelMap[name].reset(new Model(mesh, material));
	}
	return true;
}

bool ResourceManager::loadMesh(const std::string& name)
{
	if (d_meshMap.count(name) == 0)
	{
		std::string meshFile = d_resourcePath + "mesh/" + name + ".obj";
		SPDLOG_DEBUG("Creating new mesh {} from file {}.", name.c_str(), meshFile.c_str());

		// TODO Specialized for obj files, should be general purpose.
		// Obj loader
		ObjModelLoader loader;
		if (!loader.load(meshFile))
		{
			SPDLOG_ERROR("Failed to load mesh {} from file {}", name.c_str(), meshFile.c_str());
			return false;
		}

		// Create new and add to map
		std::unique_ptr<Mesh> mesh(new Mesh);
		if (!mesh->init(loader.getVertices(), loader.getNormals(), loader.getUV(), loader.getAABBox()))
		{
			SPDLOG_ERROR("Failed to initialize mesh object.");
			return false;
		}
		d_meshMap[name] = std::move(mesh);
	}
	return true;
}

bool ResourceManager::loadMaterial(const std::string& name)
{
	if (d_materialMap.count(name) == 0)
	{
		SPDLOG_DEBUG("Loading new material {}.", name.c_str());
		ConfigFile conf;
		std::string materialFile = d_resourcePath + "material/" + name + ".mat";
		std::unique_ptr<AMaterial> material(nullptr);

		// Load and error checking
		if (!conf.load(materialFile))
		{
			SPDLOG_ERROR("Failed to load shader file {}.", materialFile.c_str());
			return false;
		}

		// Material construction logic here.
		// TODO Should be factory
		// Requires class property
		if (!conf.has("class"))
		{
			SPDLOG_ERROR("Missing class property in material file {}.", materialFile.c_str());
			return false;
		}
		// Requires shader file
		if (!conf.has("shader"))
		{
			SPDLOG_ERROR("Missing shader property in material file {}.", materialFile.c_str());
			return false;
		}
		// Material class type id
		std::string materialClass = conf.get("class", "error");

		// Load shader
		std::string shader = conf.get("shader", "error");
		if (!loadShader(shader))
		{
			SPDLOG_ERROR("Failed to load shader file {}.", shader.c_str());
			return false;
		}

		// Create material based on class type id
		if (materialClass == "basic")
		{
			// Requires base texture
			if (!conf.has("texture0"))
			{
				SPDLOG_ERROR("Missing texture0 property in material file {}.", materialFile.c_str());
				return false;
			}

			// Optional ambient
			if (!conf.has("ambient"))
			{
				SPDLOG_WARN("Missing ambient property in material file {}.", materialFile.c_str());
			}

			// Optional ambient
			if (!conf.has("specular"))
			{
				SPDLOG_WARN("Missing specular property in material file {}.", materialFile.c_str());
			}

			// Load texture
			std::string texture = conf.get("texture0", "error");
			if (!loadTexture(texture))
			{
				SPDLOG_ERROR("Failed to load texture file {}.", texture.c_str());
				return false;
			}

			// Create material
			material.reset(new BasicMaterial(getShader(shader), conf.get("ambient", 0.1f), conf.get("specular", 0.2f), getTexture(texture)));
		}
		else if (materialClass == "color")
		{
			// Requires color
			if (!conf.has("color"))
			{
				SPDLOG_ERROR("Missing color property in material file {}.", materialFile.c_str());
				return false;
			}
			glm::vec3 color = conf.get("color", glm::vec3(1.f, 0.f, 0.f));
			material.reset(new ColorMaterial(getShader(shader), color));
		}
		else
		{
			SPDLOG_ERROR("Unknown class name property value {} in material file {}.", materialClass.c_str(), materialFile.c_str());
			return false;
		}

		// Add material to internal map
		d_materialMap[name] = std::move(material);
	}
	return true;
}

bool ResourceManager::loadTexture(const std::string& name)
{
	// Already loaded
	if (d_textureMap.count(name) == 0)
	{
		Image image;
		std::string textureFile = d_resourcePath + "texture/" + name + ".tga";
		SPDLOG_DEBUG("Loading texture {} from file.", textureFile.c_str());
		if (!image.load(textureFile))
		{
			SPDLOG_ERROR("Failed to load image file {}.", textureFile.c_str());
			return false;
		}
		std::unique_ptr<Texture> texture(new Texture);
		if (!texture->init(image))
		{
			SPDLOG_ERROR("Failed to initialize texture {} from file {}.", name.c_str(), textureFile.c_str());
			return false;
		}
		d_textureMap[name] = std::move(texture);
	}
	return true;
}

bool ResourceManager::loadShader(const std::string& name)
{
	// Already loaded?
	if (d_shaderMap.count(name) == 0)
	{
		// Shader definition files are located in the shader sub directory of the resource path
		std::string shaderFile = d_resourcePath + "shader/" + name + ".sha";
		SPDLOG_DEBUG("Loading shader {} from file {}.", name.c_str(), shaderFile.c_str());

		// Load shader config file with names of shader sources
		ConfigFile shaderConfig;
		if (!shaderConfig.load(shaderFile))
		{
			SPDLOG_ERROR("Failed to load shader {} from file {}.", name.c_str(), shaderFile.c_str());
			return false;
		}

		// Required property vertexshader
		if (!shaderConfig.has("vertexshader"))
		{
			SPDLOG_ERROR("Missing vertexshader property in shader definition file {}.", shaderFile.c_str());
			return false;
		}
		// Required property fragmentshader
		if (!shaderConfig.has("fragmentshader"))
		{
			SPDLOG_ERROR("Missing fragmentshader property in shader definition file {}.", shaderFile.c_str());
			return false;
		}

		// Create path to the sources. 
		// Shader source files are located at the src/ directory in the shader path
		std::string vertexShader(d_resourcePath + "shader/src/" + shaderConfig.get("vertexshader", "error"));
		std::string fragmentShader(d_resourcePath + "shader/src/" + shaderConfig.get("fragmentshader", "error"));
		
		std::unique_ptr<Shader> shader(new Shader);
		if (!shader->init(vertexShader, fragmentShader))
		{
			SPDLOG_ERROR("Failed to initialize shader object from vertex shader {} and fragment shader {}.", vertexShader.c_str(), fragmentShader.c_str());
			return false;
		}
		// Add shader to map
		d_shaderMap[name] = std::move(shader);
	}
	return true;
}

Shader* ResourceManager::getShader(const std::string& name)
{
	const auto& shader = d_shaderMap.find(name);
	if (shader == d_shaderMap.end())
	{
		SPDLOG_INFO("The requested shader {} is currently not loaded.", name.c_str());
		if (loadShader(name))
		{
			return d_shaderMap.at(name).get();
		}
		SPDLOG_ERROR("The requested shader {} does not exist", name.c_str());
		return nullptr;
	}
	return shader->second.get();
}

Texture* ResourceManager::getTexture(const std::string& name)
{
	const auto& texture = d_textureMap.find(name);
	if (texture == d_textureMap.end())
	{
		SPDLOG_WARN("The requested texture {} is currently not loaded.", name.c_str());
		if (loadTexture(name))
		{
			return d_textureMap.at(name).get();
		}
		SPDLOG_ERROR("The requested texture {} does not exist", name.c_str());
		return nullptr;
	}
	return texture->second.get();
}

Mesh* ResourceManager::getMesh(const std::string& meshName)
{
	const auto& mesh = d_meshMap.find(meshName);
	if (mesh == d_meshMap.end())
	{
		SPDLOG_WARN("The requested mesh {} is currently not loaded.", meshName.c_str());
		if (loadMesh(meshName))
		{
			return d_meshMap.at(meshName).get();
		}
		SPDLOG_ERROR("The requested mesh {} does not exist", meshName.c_str());
		return nullptr;
	}
	return mesh->second.get();
}

AMaterial* ResourceManager::getMaterial(const std::string& name)
{
	const auto& material = d_materialMap.find(name);
	if (material == d_materialMap.end())
	{
		SPDLOG_WARN("The requested material {} is currently not loaded.", name.c_str());
		if (loadMaterial(name))
		{
			return d_materialMap.at(name).get();
		}
		SPDLOG_ERROR("The requested material {} does not exist", name.c_str());
		return nullptr;
	}
	return material->second.get();
}

Model* ResourceManager::getModel(const std::string& name)
{
	const auto& model = d_modelMap.find(name);
	if (model == d_modelMap.end())
	{
		SPDLOG_WARN("The requested model {} is currently not loaded.", name.c_str());
		if (loadModel(name))
		{
			return d_modelMap.at(name).get();
		}
		SPDLOG_ERROR("The requested model {} does not exist", name.c_str());
		return nullptr;
	}
	return model->second.get();
}
