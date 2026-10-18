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

// Distance from the camera, used to fade into the horizon colour.
out float _fogDepth;

void main()
{
	mat4 model = translation * scale * rotation;
	vec4 viewPosition = view * model * vec4(vertex, 1.f);

	_fogDepth = -viewPosition.z;

	gl_Position = proj * viewPosition;
}