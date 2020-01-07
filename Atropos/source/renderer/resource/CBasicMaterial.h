#pragma once

#include "AMaterial.h"

class CTexture;

/**
* \brief A basic material class.
*
* The basic material holds a single texture unit and a phong shader
* with the required uniform parameters.
*/
class CBasicMaterial : public AMaterial
{
public:
	/**
	* \brief Creates material from shader, shader parameters and texture.
	*/
	CBasicMaterial(CShader* shader, float ambient, float specular, CTexture* texture);
	
	/**
	* \brief Returns ambient term.
	*/
	float getAmbient() const;

	/**
	* \brief Returns specular term.
	*/
	float getSpecular() const;
	
	/**
	* \brief Sets ambient term.
	*/
	void setAmbient(float);

	/**
	* \brief Sets specular term.
	*/
	void setSpecular(float);

	/**
	* \brief Sets uniform values and binds shader.
	*/
	virtual void setActive() const;
	
private:
	/**
	* \brief Model texture.
	*/
	CTexture* d_texture;

	/**
	* \brief Ambient term.
	*/
	float d_ambient;

	/**
	* \brief Specular term.
	*/
	float d_specular;
};