#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

out vec4 _color;

void main()
{
	// Full screen clip space quad; depth 1 puts it at the far plane.
	gl_Position = vec4(position, 1.f, 1.f);
	_color = color;
}