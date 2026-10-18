#version 450 core

uniform sampler2D source;
uniform float threshold;

in vec2 _uv;

out vec4 color;

void main()
{
	vec2 texel = 1.0 / vec2(textureSize(source, 0));

	vec3 scene = texture(source, _uv).rgb;

	// Weighted by channel rather than flat, so a saturated orange fireball keeps its
	// hue in the halo instead of blooming towards white.
	float weight = max(scene.r, max(scene.g, scene.b));
	weight = max(weight - threshold, 0.f) / max(weight, 0.0001);

	// Halves resolution on the way out, so averaging the four taps is both the
	// downsample and the first softening of the bright pass.
	vec3 sum = texture(source, _uv + vec2(-0.5, -0.5) * texel).rgb;
	sum += texture(source, _uv + vec2(0.5, -0.5) * texel).rgb;
	sum += texture(source, _uv + vec2(-0.5, 0.5) * texel).rgb;
	sum += texture(source, _uv + vec2(0.5, 0.5) * texel).rgb;

	color = vec4(sum * 0.25 * weight, 1.f);
}