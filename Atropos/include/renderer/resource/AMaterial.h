#pragma once

class Shader;

class AMaterial
{
public:
	virtual ~AMaterial();
	
	virtual void setActive() const = 0;

	const Shader* getShader() const;

protected:
	AMaterial(Shader* shader);

	Shader* d_shader;
};