#version 450 core

in vec2 _uv;
in vec4 _color;

uniform sampler2D atlas;
uniform bool useAtlas;

out vec4 color;

void main()
{
	color = useAtlas ? _color * texture(atlas, _uv) : _color;
}