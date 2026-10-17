#version 330 core

// Passthrough shader for 2D / fullscreen quad drawing

// Vertex data
in vec3 vertex;

void main()
{
	gl_Position = vec4(vertex, 1.f);
}