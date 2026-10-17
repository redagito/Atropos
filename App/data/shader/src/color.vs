#version 330 core

// Set by model
in vec3 vertex;
in vec3 normal;
in vec2 uv;

// Set by renderer
uniform mat4 rotation;
uniform mat4 translation;
uniform mat4 scale;

uniform mat4 view;
uniform mat4 proj;

out vec3 _color;

void main()
{
	mat4 model = translation * scale * rotation;

	gl_Position = proj * view * model * vec4(vertex, 1.f);
}