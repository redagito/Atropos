#pragma once

// Forward declaration
class Shader;

/**
* \brief Surface rendering information for models.
*
* A material specifies the surface properties of a model.
* Common properties are shader, shader attributes and textures.
*/
class AMaterial
{
public:
	/**
	* \brief Virtual destructor for abstract base class.
	*/
	virtual ~AMaterial();
	
	/**
	* \brief Sets the material as active for rendering
	*/
	virtual void setActive() const = 0;

	/**
	* \brief Read access to the material shader.
	*/
	const Shader* getShader() const;

protected:
	/**
	* \brief Protected constructor for abstract base class.
	*/
	AMaterial(Shader* shader);

	/**
	* \brief The material shader.
	*/
	Shader* d_shader;
};