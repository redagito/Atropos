#pragma once

#include "CShader.h"
#include "CMesh.h"
#include "CTexture.h"
#include "CModel.h"
#include "AMaterial.h"

#include "core/TFactory.h"

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
class CResourceManager
{
public:
	typedef TFactory<AMaterial, std::string> MaterialFactory; /**< Material factory. */

	/**
	* \brief Initialization for the resource manager.
	*
	* Sets the resource path to be searched.
	*/
	CResourceManager(const std::string& resourceBasePath);

	/**
	* \brief Cleanup of resources
	* 
	* Deletes all loaded resources. Any pointer returned by this class is invalid after destructor call.
	*/
	~CResourceManager();

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
	CModel* getModel(const std::string& name);

	/**
	* \brief Returns pointer to requested texture resource.
	* \param name The name of the texture object.
	*/
	CTexture* getTexture(const std::string& name);

	/**
	* \brief Returns pointer to requested shader resource.
	* \param name The name of the shader object.
	*/
	CShader* getShader(const std::string& name);

	/**
	* \brief Returns pointer to requested mesh resource.
	* \param name Name of the mesh file.
	*/
	CMesh* getMesh(const std::string& name);

private:
	std::string d_resourcePath; /**< Base resource path. */
	MaterialFactory d_materialFactory; /**< Internal material factory. */

	std::unordered_map<std::string, std::unique_ptr<CTexture>> d_textureMap; /**< Loaded textures. */
	std::unordered_map<std::string, std::unique_ptr<CShader>> d_shaderMap; /**< Loaded shader. */
	std::unordered_map<std::string, std::unique_ptr<CMesh>> d_meshMap; /**< Loaded meshes. */
	std::unordered_map<std::string, std::unique_ptr<AMaterial>> d_materialMap; /**< Loaded materials. */
	std::unordered_map<std::string, std::unique_ptr<CModel>> d_modelMap; /**< Loaded models. */
};
