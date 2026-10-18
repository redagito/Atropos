#version 450 core

uniform sampler2D source;
uniform sampler2D blur;
uniform float strength;

in vec2 _uv;

out vec4 color;

void main()
{
	vec3 scene = texture(source, _uv).rgb;
	vec3 halo = texture(blur, _uv).rgb;

	// The default framebuffer is 8 bit, so anything past one is written as white.
	color = vec4(scene + halo * strength, 1.f);
}