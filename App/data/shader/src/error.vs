#version 330 core

in vec3 vertex;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 _normal;

void main()
{
	gl_Position =  proj * view * model * vec4(vertex, 1.0);
	_normal = normal;
}