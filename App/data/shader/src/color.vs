#version 450 core

// Set by model
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

// Set by renderer
uniform mat4 rotation;
uniform mat4 translation;
uniform mat4 scale;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	mat4 model = translation * scale * rotation;

	gl_Position = proj * view * model * vec4(vertex, 1.f);
}