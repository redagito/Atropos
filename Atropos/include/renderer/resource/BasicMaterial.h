#pragma once

#include "renderer/resource/AMaterial.h"

class Texture;

/**
* \brief A basic material class.
*
* Holds a single texture and a phong shader with the required uniform parameters.
*/
class BasicMaterial : public AMaterial
{
public:
	BasicMaterial(Shader* shader, float ambient, float specular, Texture* texture);

	virtual void setActive() const;
	
private:
	Texture* d_texture;

	float d_ambient;

	float d_specular;
};