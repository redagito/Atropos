#pragma once

#include "renderer/resource/Shader.h"
#include "renderer/resource/Mesh.h"
#include "renderer/resource/Texture.h"
#include "renderer/resource/Model.h"
#include "renderer/resource/AMaterial.h"
#include "renderer/resource/MeshData.h"
#include "renderer/resource/ShaderSources.h"

#include "resource/Image.h"

#include "core/AsyncLoader.h"

#include "glm/glm.hpp"

#include <string>
#include <memory>

class ThreadPool;

/**
* \brief CPU side material definition.
*/
struct MaterialDesc
{
	std::string materialClass;
	std::string shader;
	/**< Empty for color materials. */
	std::string texture;
	float ambient = 0.1f;
	float specular = 0.2f;
	/**< Only for color materials. */
	glm::vec3 color{ 0.f };
};

/**
* \brief CPU side model definition.
*/
struct ModelDesc
{
	std::string mesh;
	std::string material;
};

/**
* \brief Loads and manages game resources asynchronously.
*
* The thread pool must outlive the resource manager, because a queued parse
* task holds a raw pointer to it and the pool drains its queue when it stops.
*/
class ResourceManager
{
public:
	ResourceManager(const std::string& resourceBasePath, ThreadPool& pool);

	~ResourceManager();

	void loadModel(const std::string& name);

	void loadMesh(const std::string& name);

	void loadMaterial(const std::string& name);

	void loadTexture(const std::string& name);

	void loadShader(const std::string& name);

	void process();

	/**
	* \brief True when all requested resources are loaded or failed.
	*/
	bool isIdle() const;

	bool hasError() const;

	/**
	* \brief Loads on demand.
	* Throws std::runtime_error if the material cannot be loaded.
	*/
	AMaterial* getMaterial(const std::string& name);

	/**
	* \brief Loads on demand.
	* Throws std::runtime_error if the model cannot be loaded.
	*/
	Model* getModel(const std::string& name);

	/**
	* \brief Loads on demand.
	* Throws std::runtime_error if the texture cannot be loaded.
	*/
	Texture* getTexture(const std::string& name);

	/**
	* \brief Loads on demand.
	* Throws std::runtime_error if the shader cannot be loaded.
	*/
	Shader* getShader(const std::string& name);

	/**
	* \brief Loads on demand.
	* Throws std::runtime_error if the mesh cannot be loaded.
	*/
	Mesh* getMesh(const std::string& name);

private:
	ShaderSources parseShader(const std::string& name) const;
	MeshData parseMesh(const std::string& name) const;
	Image parseTexture(const std::string& name) const;
	MaterialDesc parseMaterial(const std::string& name) const;
	ModelDesc parseModel(const std::string& name) const;

	Shader* lookupShader(const std::string& name) const;
	Texture* lookupTexture(const std::string& name) const;
	Mesh* lookupMesh(const std::string& name) const;
	AMaterial* lookupMaterial(const std::string& name) const;

	template <typename T, typename Parsed>
	T* waitFor(AsyncLoader<T, Parsed>& loader, const std::string& name);

	std::string d_resourcePath;

	AsyncLoader<Shader, ShaderSources> d_shaders;
	AsyncLoader<Texture, Image> d_textures;
	AsyncLoader<Mesh, MeshData> d_meshes;
	AsyncLoader<AMaterial, MaterialDesc> d_materials;
	AsyncLoader<Model, ModelDesc> d_models;
};
