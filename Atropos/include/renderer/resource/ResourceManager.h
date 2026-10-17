#pragma once

#include "renderer/resource/Shader.h"
#include "renderer/resource/Mesh.h"
#include "renderer/resource/Texture.h"
#include "renderer/resource/Model.h"
#include "renderer/resource/AMaterial.h"

#include <string>
#include <unordered_map>
#include <memory>

/**
* \brief Loads and manages game resources.
*
* Resource manager for on-demand loading of resources like textures, meshes, shader programs.
* This should probably be a singleton, but using more than one of these, even for the same resources
* should not cause any problems except for higher memory usage due to redundant data.
* The resource lifetimes are managed by this class. Calling delete on the returned pointers
* causes undefined behavior.
* TODO: Implement proxy pattern for returned resources to eliminate this issue.
*/
class ResourceManager
{
public:
	/**
	* \brief Initialization for the resource manager.
	*
	* Sets the resource path to be searched.
	*/
	ResourceManager(const std::string& resourceBasePath);

	/**
	* \brief Cleanup of resources
	* 
	* Deletes all loaded resources. Any pointer returned by this class is invalid after destructor call.
	*/
	~ResourceManager();

	/**
	* \brief Loads model resource.
	*/
	bool loadModel(const std::string& name);
	
	/**
	* \brief Loads mesh resource.
	*/
	bool loadMesh(const std::string& name);

	/**
	* \brief Loads material resource.
	*/
	bool loadMaterial(const std::string& name);
	
	/**
	* \brief Loads texture resource.
	*/
	bool loadTexture(const std::string& name);
	
	/**
	* \brief Loads shader resource.
	*/
	bool loadShader(const std::string& name);

	/**
	* \brief Returns pointer to requested material resource.
	* \param name Material name.
	*/
	AMaterial* getMaterial(const std::string& name);

	/**
	* \brief Returns pointer to requested model resource.
	* \param name Model name.
	*/
	Model* getModel(const std::string& name);

	/**
	* \brief Returns pointer to requested texture resource.
	* \param name The name of the texture object.
	*/
	Texture* getTexture(const std::string& name);

	/**
	* \brief Returns pointer to requested shader resource.
	* \param name The name of the shader object.
	*/
	Shader* getShader(const std::string& name);

	/**
	* \brief Returns pointer to requested mesh resource.
	* \param name Name of the mesh file.
	*/
	Mesh* getMesh(const std::string& name);

private:
	std::string d_resourcePath; /**< Base resource path. */

	std::unordered_map<std::string, std::unique_ptr<Texture>> d_textureMap; /**< Loaded textures. */
	std::unordered_map<std::string, std::unique_ptr<Shader>> d_shaderMap; /**< Loaded shader. */
	std::unordered_map<std::string, std::unique_ptr<Mesh>> d_meshMap; /**< Loaded meshes. */
	std::unordered_map<std::string, std::unique_ptr<AMaterial>> d_materialMap; /**< Loaded materials. */
	std::unordered_map<std::string, std::unique_ptr<Model>> d_modelMap; /**< Loaded models. */
};
