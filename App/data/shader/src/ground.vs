#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

out vec3 out_normal;
out vec3 worldPos;

void main()
{
	gl_Position =  Proj * View * Model * vec4(vertex, 1.0);
	out_normal = normal;
	worldPos = vertex;
}