#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

out vec2 _uv;
out vec4 _color;

// Positions arrive already in clip space, so no projection matrix is needed.
void main()
{
	gl_Position = vec4(position, 0.f, 1.f);
	_uv = uv;
	_color = color;
}