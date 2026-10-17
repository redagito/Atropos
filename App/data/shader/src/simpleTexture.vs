#version 330 core

in vec3 vertex;
in vec2 uv;

uniform mat4 rotation;
uniform mat4 translation;
uniform mat4 scale;

uniform mat4 view;
uniform mat4 proj;

out vec2 _uv;

void main()
{	
	mat4 model = translation * scale * rotation;
	gl_Position = proj * view * model * vec4(vertex, 1);
	_uv = uv;
}