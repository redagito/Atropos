#version 450 core

// Set by model
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

// Set by renderer
uniform mat4 rotation;
uniform mat4 model;

uniform mat4 view;
uniform mat4 proj;

// For shadow mapping
uniform mat4 bias; // Bias matrix

out vec2 _uv;
out vec3 _normal;

// For shadow mapping
out vec4 _shadowCoord;

// Distance from the camera, used to fade into the horizon colour.
out float _fogDepth;

void main()
{
	_shadowCoord = bias * vec4(vertex, 1.f);

	vec4 viewPosition = view * model * vec4(vertex, 1.f);
	_fogDepth = -viewPosition.z;

	gl_Position = proj * viewPosition;

	_normal = (rotation * vec4(normal, 0.f)).xyz;
	_uv = uv;
}