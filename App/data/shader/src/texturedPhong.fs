#version 330 core

// Set by material
uniform sampler2D texture0;

in vec2 _uv;
smooth in vec3 _normal;
in float _ambient;
in float _specular;
in float _diffuse;
in float _height;

// For shadow mapping
in vec4 _shadowCoord;
in vec3 _vertexWorld; // Vertex position in world space
in vec3 _cameraDir; // Vector from vertex to camera
in vec3 _lightDir; // Vector from vertex to light
in vec3 _normalCamera; // Vertex normal in camera space
uniform sampler2DShadow shadow;

out vec4 color;

// Shadow sampling
vec2 poissonDisk[16] = vec2[]( 
	vec2( -0.94201624, -0.39906216 ), 
	vec2( 0.94558609, -0.76890725 ), 
	vec2( -0.094184101, -0.92938870 ), 
	vec2( 0.34495938, 0.29387760 ), 
	vec2( -0.91588581, 0.45771432 ), 
	vec2( -0.81544232, -0.87912464 ), 
	vec2( -0.38277543, 0.27676845 ), 
	vec2( 0.97484398, 0.75648379 ), 
	vec2( 0.44323325, -0.97511554 ), 
	vec2( 0.53742981, -0.47373420 ), 
	vec2( -0.26496911, -0.41893023 ), 
	vec2( 0.79197514, 0.19090188 ), 
	vec2( -0.24188840, 0.99706507 ), 
	vec2( -0.81409955, 0.91437590 ), 
	vec2( 0.19984126, 0.78641367 ), 
	vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main()
{
	// Shadow
	vec3 n = normalize(_normalCamera);
	vec3 l = normalize(_lightDir);
	vec3 e = normalize(_cameraDir);
	vec3 r = reflect(-l, n);
	float cosAlpha = clamp(dot(e, r), 0.f, 1.f);
	float cosTheta = clamp(dot(n, l), 0.f, 1.f);
	float visible = 1.f;
	float bias = 0.005f;

	// Poisson sampling
	for (int i = 0; i < 2; ++i)
	{
		int index = int(2.f * random(gl_FragCoord.xyy, i)) % 2;
		visible -= 0.4f * (1.f - texture(shadow, vec3(_shadowCoord.xy + poissonDisk[index] / 500, (_shadowCoord.z - bias) / _shadowCoord.w)));
	}

	vec3 lightDir = vec3(0.0, -1.0, 0.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec4 baseColor = texture(texture0, _uv);
	float diffuseFactor = max(0.0, dot(normalize(_normal), -lightDir));
	color = baseColor * vec4(lightColor * (0.3 + diffuseFactor), 1.0)  * visible;
	color.a = 1;
}