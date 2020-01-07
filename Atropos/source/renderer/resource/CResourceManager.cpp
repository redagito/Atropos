#include "CResourceManager.h"

#include "io/CObjModelLoader.h"
#include "io/CConfigFile.h"

#include "CBasicMaterial.h"
#include "CColorMaterial.h"

#include "resource/CImage.h"
#include "debug/Log.h"

// Custom memory manager


CResourceManager::CResourceManager(const std::string& resourceBasePath)
	:
	d_resourcePath(resourceBasePath)
{
	
}

CResourceManager::~CResourceManager()
{

}

bool CResourceManager::loadModel(const std::string& name)
{
	if (d_modelMap.count(name) == 0)
	{
		LOG_DEBUG("Loading model: %s.", name.c_str());
		// Model file name
		std::string modelFile(d_resourcePath + "model/" + name + ".mdl");
		// Model config
		CConfigFile conf;
		// Load and error checking
		if (!conf.load(((d_resourcePath + "model/" + name + ".mdl"))))
		{
			LOG_ERROR("Failed to load model %s from file %s.", name.c_str(), modelFile.c_str());
			return false;
		}
		// Load into map
		if (!conf.has("mesh"))
		{
			LOG_ERROR("Missing mesh parameter in model file %s.", modelFile.c_str());
			return false;
		}
		if (!conf.has("material"))
		{
			LOG_ERROR("Missing material parameter in model file %s.", modelFile.c_str());
			return false;
		}
		if (!loadMesh(conf.get("mesh", "error")))
		{
			return false;
		}

		d_modelMap[name].reset(new CModel(getMesh(conf.get("mesh", "error")), getMaterial(conf.get("material", "error"))));
	}
	return true;
}

bool CResourceManager::loadMesh(const std::string& name)
{
	if (d_meshMap.count(name) == 0)
	{
		std::string meshFile = d_resourcePath + "mesh/" + name + ".obj";
		LOG_DEBUG("Creating new mesh %s from file %s.", name.c_str(), meshFile.c_str());

		// TODO Specialized for obj files, should be general purpose.
		// Obj loader
		CObjModelLoader loader;
		if (!loader.load(meshFile))
		{
			LOG_ERROR("Failed to load mesh %s from file %s", name.c_str(), meshFile.c_str());
			return false;
		}

		// Create new and add to map
		std::unique_ptr<CMesh> mesh(new CMesh);
		if (!mesh->init(loader.getVertices(), loader.getNormals(), loader.getUV(), loader.getAABBox()))
		{
			LOG_ERROR("Failed to initialize mesh object.");
			return false;
		}
		d_meshMap[name] = std::move(mesh);
	}
	return true;
}

bool CResourceManager::loadMaterial(const std::string& name)
{
	if (d_materialMap.count(name) == 0)
	{
		LOG_DEBUG("Loading new material %s.", name.c_str());
		CConfigFile conf;
		std::string materialFile = d_resourcePath + "material/" + name + ".mat";
		std::unique_ptr<AMaterial> material(nullptr);

		// Load and error checking
		if (!conf.load(materialFile))
		{
			LOG_ERROR("Failed to load shader file %s.", materialFile.c_str());
			return false;
		}

		// Material construction logic here.
		// TODO Should be factory
		// Requires class property
		if (!conf.has("class"))
		{
			LOG_ERROR("Missing class property in material file %s.", materialFile.c_str());
			return false;
		}
		// Requires shader file
		if (!conf.has("shader"))
		{
			LOG_ERROR("Missing shader property in material file %s.", materialFile.c_str());
			return false;
		}
		// Material class type id
		std::string materialClass = conf.get("class", "error");

		// Load shader
		std::string shader = conf.get("shader", "error");
		if (!loadShader(shader))
		{
			LOG_ERROR("Failed to load shader file %s.", shader.c_str());
			return false;
		}

		// Create material based on class type id
		if (materialClass == "basic")
		{
			// Requires base texture
			if (!conf.has("texture0"))
			{
				LOG_ERROR("Missing texture0 property in material file %s.", materialFile.c_str());
				return false;
			}

			// Optional ambient
			if (!conf.has("ambient"))
			{
				LOG_WARNING("Missing ambient property in material file %s.", materialFile.c_str());
			}

			// Optional ambient
			if (!conf.has("specular"))
			{
				LOG_WARNING("Missing specular property in material file %s.", materialFile.c_str());
			}

			// Load texture
			std::string texture = conf.get("texture0", "error");
			if (!loadTexture(texture))
			{
				LOG_ERROR("Failed to load texture file %s.", texture.c_str());
				return false;
			}

			// Create material
			material.reset(new CBasicMaterial(getShader(shader), conf.get("ambient", 0.1f), conf.get("specular", 0.2f), getTexture(texture)));
		}
		else if (materialClass == "color")
		{
			// Requires color
			if (!conf.has("color"))
			{
				LOG_ERROR("Missing color property in material file %s.", materialFile.c_str());
				return false;
			}
			glm::vec3 color = conf.get("color", glm::vec3(1.f, 0.f, 0.f));
			material.reset(new CColorMaterial(getShader(shader), color));
		}
		else
		{
			LOG_ERROR("Unknown class name property value %s in material file %s.", materialClass.c_str(), materialFile.c_str());
			return false;
		}

		// Add material to internal map
		d_materialMap[name] = std::move(material);
	}
	return true;
}

bool CResourceManager::loadTexture(const std::string& name)
{
	// Already loaded
	if (d_textureMap.count(name) == 0)
	{
		CImage image;
		std::string textureFile = d_resourcePath + "texture/" + name + ".tga";
		LOG_DEBUG("Loading texture %s from file.", textureFile.c_str());
		if (!image.load(textureFile))
		{
			LOG_ERROR("Failed to load image file %s.", textureFile.c_str());
			return false;
		}
		std::unique_ptr<CTexture> texture(new CTexture);
		if (!texture->init(image))
		{
			LOG_ERROR("Failed to initialize texture %s from file %s.", name.c_str(), textureFile.c_str());
			return false;
		}
		d_textureMap[name] = std::move(texture);
	}
	return true;
}

bool CResourceManager::loadShader(const std::string& name)
{
	// Already loaded?
	if (d_shaderMap.count(name) == 0)
	{
		// Shader definition files are located in the shader sub directory of the resource path
		std::string shaderFile = d_resourcePath + "shader/" + name + ".sha";
		LOG_DEBUG("Loading shader %s from file &s.", name.c_str(), shaderFile.c_str());

		// Load shader config file with names of shader sources
		CConfigFile shaderConfig;
		if (!shaderConfig.load(shaderFile))
		{
			LOG_ERROR("Failed to load shader %s from file %s.", name.c_str(), shaderFile.c_str());
			return false;
		}

		// Required property vertexshader
		if (!shaderConfig.has("vertexshader"))
		{
			LOG_ERROR("Missing vertexshader property in shader definition file %s.", shaderFile.c_str());
			return false;
		}
		// Required property fragmentshader
		if (!shaderConfig.has("fragmentshader"))
		{
			LOG_ERROR("Missing fragmentshader property in shader definition file %s.", shaderFile.c_str());
			return false;
		}

		// Create path to the sources. 
		// Shader source files are located at the src/ directory in the shader path
		std::string vertexShader(d_resourcePath + "shader/src/" + shaderConfig.get("vertexshader", "error"));
		std::string fragmentShader(d_resourcePath + "shader/src/" + shaderConfig.get("fragmentshader", "error"));
		
		std::unique_ptr<CShader> shader(new CShader);
		if (!shader->init(vertexShader, fragmentShader))
		{
			LOG_ERROR("Failed to initialize shader object from vertex shader %s and fragment shader %s.", vertexShader.c_str(), fragmentShader.c_str());
			return false;
		}
		// Add shader to map
		d_shaderMap[name] = std::move(shader);
	}
	return true;
}

CShader* CResourceManager::getShader(const std::string& name)
{
	const auto& shader = d_shaderMap.find(name);
	if (shader == d_shaderMap.end())
	{
		LOG_WARNING("The requested shader %s was not loaded.", name.c_str());
		if (loadShader(name))
		{
			return d_shaderMap.at(name).get();
		}
		LOG_ERROR("The requested shader %s does not exist", name.c_str());
		return nullptr;
	}
	return shader->second.get();
}

CTexture* CResourceManager::getTexture(const std::string& name)
{
	const auto& texture = d_textureMap.find(name);
	if (texture == d_textureMap.end())
	{
		LOG_WARNING("The requested texture %s was not loaded.", name.c_str());
		if (loadTexture(name))
		{
			return d_textureMap.at(name).get();
		}
		LOG_ERROR("The requested texture %s does not exist", name.c_str());
		return nullptr;
	}
	return texture->second.get();
}

CMesh* CResourceManager::getMesh(const std::string& meshName)
{
	const auto& mesh = d_meshMap.find(meshName);
	if (mesh == d_meshMap.end())
	{
		LOG_WARNING("The requested mesh %s was not loaded.", meshName.c_str());
		if (loadMesh(meshName))
		{
			return d_meshMap.at(meshName).get();
		}
		LOG_ERROR("The requested mesh %s does not exist", meshName.c_str());
		return nullptr;
	}
	return mesh->second.get();
}

AMaterial* CResourceManager::getMaterial(const std::string& name)
{
	const auto& material = d_materialMap.find(name);
	if (material == d_materialMap.end())
	{
		LOG_WARNING("The requested material %s was not loaded.", name.c_str());
		if (loadMaterial(name))
		{
			return d_materialMap.at(name).get();
		}
		LOG_ERROR("The requested material %s does not exist", name.c_str());
		return nullptr;
	}
	return material->second.get();
}

CModel* CResourceManager::getModel(const std::string& name)
{
	const auto& model = d_modelMap.find(name);
	if (model == d_modelMap.end())
	{
		LOG_WARNING("The requested model %s was not loaded.", name.c_str());
		if (loadModel(name))
		{
			return d_modelMap.at(name).get();
		}
		LOG_ERROR("The requested model %s does not exist", name.c_str());
		return nullptr;
	}
	return model->second.get();
}