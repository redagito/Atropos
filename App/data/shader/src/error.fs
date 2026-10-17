#version 330 core

// Interpolated values from the vertex shaders
in vec3 _normal;

// Ouput data
out vec3 color;


void main()
{
	color = vec3((out_normal.x + out_normal.y + out_normal.z) / 3, 0.0, 0.0);
}