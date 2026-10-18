#version 450 core

// Set by model
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

// Set by renderer
uniform mat4 rotation;
uniform mat4 translation;
uniform mat4 scale;
uniform mat4 model;

uniform mat4 view;
uniform mat4 proj;

// For shadow mapping
uniform mat4 bias; // Bias matrix
uniform vec3 lightInvDir; // Light source

// Set by material
uniform float ambient;
uniform float specular;

// For lighting
out vec2 _uv;
out vec3 _normal;
out float _ambient;
out float _specular;

// For shadow mapping
out vec4 _shadowCoord;
out vec3 _vertexWorld; // Vertex position in world space
out vec3 _cameraDir; // Vector from vertex to camera
out vec3 _lightDir; // Vector from vertex to light
out vec3 _normalCamera; // Vertex normal in camera space

void main()
{
	_shadowCoord = bias * vec4(vertex, 1.f);
	_vertexWorld = (model * vec4(vertex, 1.f)).xyz;
	_cameraDir = vec3(0.f, 0.f, 0.f) - (view * model * vec4(vertex, 1.f)).xyz;
	_lightDir = (view * vec4(lightInvDir, 0.f)).xyz;
	_normalCamera = (view * translation * rotation * vec4(normal, 0.f)).xyz;

	gl_Position = proj * view * model * vec4(vertex, 1.f);
	_normal = (rotation * vec4(normal, 0.f)).xyz;
	_uv = uv;
	_specular = specular;
	_ambient = ambient;
}