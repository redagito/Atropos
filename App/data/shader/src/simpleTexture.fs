#version 330 core

// Set by material
uniform sampler2D texture0;

in vec2 _uv;

out vec4 color;

void main()
{
    color = texture(texture0, _uv).rgba;
}