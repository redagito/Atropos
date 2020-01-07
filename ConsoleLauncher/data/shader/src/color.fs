#version 330 core

// Set by material
uniform vec3 color;

// Ouput data
out vec4 _color;

void main()
{
	_color = vec4(color, 1.f);
}