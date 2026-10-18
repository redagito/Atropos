#version 450 core

// Set by material
uniform vec3 color;

// Set by renderer
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;

in float _fogDepth;

out vec4 _color;

void main()
{
	vec4 shaded = vec4(color, 1.f);
	float fog = clamp((_fogDepth - fogStart) / max(fogEnd - fogStart, 0.001), 0.0, 1.0);
	_color = mix(shaded, vec4(fogColor, shaded.a), fog);
}