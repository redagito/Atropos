#version 450 core

layout(location = 0) in vec2 position;

out vec2 _uv;

void main()
{
	// Full screen clip space quad; the bloom stages all sample the scene through it.
	gl_Position = vec4(position, 0.f, 1.f);
	_uv = position * 0.5 + 0.5;
}