#version 450 core

uniform sampler2D source;

uniform vec2 direction;

in vec2 _uv;

out vec4 color;

// Nine tap Gaussian collapsed onto five samples, using the pairs of texels that
// linear filtering averages together.
const float WEIGHT_CENTRE = 0.2270270270;
const float WEIGHT_NEAR = 0.3162162162;
const float WEIGHT_FAR = 0.0702702703;
const float OFFSET_NEAR = 1.3846153846;
const float OFFSET_FAR = 3.2307692308;

void main()
{
	vec2 step = direction / vec2(textureSize(source, 0));

	vec3 sum = texture(source, _uv).rgb * WEIGHT_CENTRE;
	sum += texture(source, _uv + step * OFFSET_NEAR).rgb * WEIGHT_NEAR;
	sum += texture(source, _uv - step * OFFSET_NEAR).rgb * WEIGHT_NEAR;
	sum += texture(source, _uv + step * OFFSET_FAR).rgb * WEIGHT_FAR;
	sum += texture(source, _uv - step * OFFSET_FAR).rgb * WEIGHT_FAR;

	color = vec4(sum, 1.f);
}