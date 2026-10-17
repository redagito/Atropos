#pragma once

#include "renderer/resource/AMaterial.h"

class Texture;

/**
* \brief A basic material class.
*
* The basic material holds a single texture unit and a phong shader
* with the required uniform parameters.
*/
class BasicMaterial : public AMaterial
{
public:
	/**
	* \brief Creates material from shader, shader parameters and texture.
	*/
	BasicMaterial(Shader* shader, float ambient, float specular, Texture* texture);

	/**
	* \brief Sets uniform values and binds shader.
	*/
	virtual void setActive() const;
	
private:
	/**
	* \brief Model texture.
	*/
	Texture* d_texture;

	/**
	* \brief Ambient term.
	*/
	float d_ambient;

	/**
	* \brief Specular term.
	*/
	float d_specular;
};