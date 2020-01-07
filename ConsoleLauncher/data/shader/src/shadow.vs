#version 330 core

// Set by model
in vec3 vertex;
in vec3 normal;
in vec2 uv;

// Set by renderer
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(vertex, 1.f);
}