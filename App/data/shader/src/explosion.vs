#version 450 core

layout(location = 0) in vec2 corner;
layout(location = 1) in vec3 position;
layout(location = 2) in float age;
layout(location = 3) in float lifetime;
layout(location = 4) in float size;
layout(location = 5) in float seed;

uniform mat4 view;
uniform mat4 proj;

out vec2 _corner;
out float _age;
out float _seed;

void main()
{
	// Rows of the view matrix are the world space camera basis, so this faces the
	// particle at the camera without a CPU side billboard.
	vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 up = vec3(view[0][1], view[1][1], view[2][1]);

	vec3 world = position + (right * corner.x + up * corner.y) * size;

	gl_Position = proj * view * vec4(world, 1.f);

	_corner = corner;
	_age = age / lifetime;
	_seed = seed;
}