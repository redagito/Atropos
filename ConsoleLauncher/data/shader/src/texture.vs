#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;

out vec2 uv;

void main()
{
	gl_Position = MVP * vec4(vertexPos, 1.f);
	uv = vertexUV;
}