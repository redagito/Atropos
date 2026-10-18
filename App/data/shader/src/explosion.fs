#version 450 core

in vec2 _corner;
in float _age;
in float _seed;

out vec4 color;

void main()
{
	float radius = length(_corner);
	if (radius > 1.f)
	{
		discard;
	}

	float falloff = 1.f - radius;
	falloff *= falloff;

	// White hot early, cooling through orange to a dark ember.
	vec3 hot = vec3(1.f, 0.95f, 0.75f);
	vec3 flame = vec3(1.f, 0.45f, 0.10f);
	vec3 ember = vec3(0.35f, 0.05f, 0.02f);

	vec3 tint = _age < 0.35f
		? mix(hot, flame, _age / 0.35f)
		: mix(flame, ember, (_age - 0.35f) / 0.65f);

	float alpha = falloff * (1.f - _age) * (0.7f + 0.3f * _seed);

	color = vec4(tint, alpha);
}