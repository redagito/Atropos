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
* \brief Parsed material definition, CPU side data for a material resource.
*/
struct MaterialDesc
{
	std::string materialClass; /**< Material class name. */
	std::string shader; /**< Shader resource name. */
	std::string texture; /**< Texture resource name, empty for color materials. */
	float ambient = 0.1f; /**< Ambient term. */
	float specular = 0.2f; /**< Specular term. */
	glm::vec3 color{ 0.f }; /**< Color for color materials. */
};

/**
* \brief Parsed model definition, CPU side data for a model resource.
*/
struct ModelDesc
{
	std::string mesh; /**< Mesh resource name. */
	std::string material; /**< Material resource name. */
};

/**
* \brief Loads and manages game resources asynchronously.
*
* Parsing runs on the thread pool, the OpenGL upload runs on the main thread
* during process(). The resource lifetimes are managed by this class.
* The resource manager must be destroyed before the thread pool it uses.
*/
class ResourceManager
{
public:
	/**
	* \brief Sets the resource path and the pool used for parsing.
	*/
	ResourceManager(const std::string& resourceBasePath, ThreadPool& pool);

	/**
	* \brief Deletes all loaded resources.
	*/
	~ResourceManager();

	/**
	* \brief Requests a model resource, loading happens asynchronously.
	*/
	void loadModel(const std::string& name);

	/**
	* \brief Requests a mesh resource, loading happens asynchronously.
	*/
	void loadMesh(const std::string& name);

	/**
	* \brief Requests a material resource, loading happens asynchronously.
	*/
	void loadMaterial(const std::string& name);

	/**
	* \brief Requests a texture resource, loading happens asynchronously.
	*/
	void loadTexture(const std::string& name);

	/**
	* \brief Requests a shader resource, loading happens asynchronously.
	*/
	void loadShader(const std::string& name);

	/**
	* \brief Commits finished resources on the main thread.
	*/
	void process();

	/**
	* \brief True when all requested resources are loaded or failed.
	*/
	bool isIdle() const;

	/**
	* \brief True if any requested resource failed to load.
	*/
	bool hasError() const;

	/**
	* \brief Returns the material, loading it if necessary.
	* Throws std::runtime_error if the material cannot be loaded.
	*/
	AMaterial* getMaterial(const std::string& name);

	/**
	* \brief Returns the model, loading it if necessary.
	* Throws std::runtime_error if the model cannot be loaded.
	*/
	Model* getModel(const std::string& name);

	/**
	* \brief Returns the texture, loading it if necessary.
	* Throws std::runtime_error if the texture cannot be loaded.
	*/
	Texture* getTexture(const std::string& name);

	/**
	* \brief Returns the shader, loading it if necessary.
	* Throws std::runtime_error if the shader cannot be loaded.
	*/
	Shader* getShader(const std::string& name);

	/**
	* \brief Returns the mesh, loading it if necessary.
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

	std::string d_resourcePath; /**< Base resource path. */

	AsyncLoader<Shader, ShaderSources> d_shaders; /**< Shader resources. */
	AsyncLoader<Texture, Image> d_textures; /**< Texture resources. */
	AsyncLoader<Mesh, MeshData> d_meshes; /**< Mesh resources. */
	AsyncLoader<AMaterial, MaterialDesc> d_materials; /**< Material resources. */
	AsyncLoader<Model, ModelDesc> d_models; /**< Model resources. */
};
