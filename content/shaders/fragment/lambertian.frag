#version 430 core

#define MAX_NUM_LIGHTS 4
#define PI 3.1415926535
#define BLACK vec4(0.0, 0.0, 0.0, 1.0)
#define KINDA_SMALL_NUMBER 1e-5
#define VERY_SMALL_NUMBER 1e-10

//layout (location = 0) uniform float uv_scale;

layout (binding = 0) uniform sampler2D albedo_map;
layout (binding = 1) uniform sampler2D normals_map;
layout (binding = 2) uniform sampler2D roughness_map;

layout (location = 0) out vec4 fs_color;

in vec3 vs_P;
in vec3 vs_N;
in vec3 vs_V;
in vec2 vs_uv;

in VS_LIGHTS
{
	vec3 L[MAX_NUM_LIGHTS];
	vec3 H[MAX_NUM_LIGHTS];
	vec4 kL[MAX_NUM_LIGHTS];
	float D[MAX_NUM_LIGHTS];
} vs_lights;

layout(std140, binding = 2) uniform Lambertian
{
	float PBRtoNPR;
	bool halfLambert;
	float halfLambertBias;
	int colorShades;
	float blendedShadePercent;
	bool hsvShift;
} lambertian;

float Attenuation(in float D);

vec4 Lambert(in vec4 kD, in vec4 kL, in float NdotL);

vec4 HalfLambert(in vec4 kD, in vec4 kL, in float NdotL);

vec4 Lambert_NPR(in vec4 kD, in vec4 kL, in float NdotL, in bool halfLambert, in float bias, in int shades, in float blendPercent, in bool hsv);

vec3 ShiftKD(in vec3 kD, in bool hsv, in float rgbShift, in float hueShift, in float valueShift);

vec3 ShiftKL(in vec3 kL, in bool hsv, in float hueShift);

vec3 RGBtoHSV(in vec3 RGB);

vec3 HSVtoRGB(in vec3 HSV);

void main()
{
	// Normalize the input vectors as they have been interpolated
	vec3 N = normalize(vs_N);
	vec3 V = normalize(vs_V);

	// Compute the fragment UVs
	//vec2 fs_uv = mod(vs_uv * uv_scale, 1.0);
	vec2 fs_uv = vs_uv;

	vec4 kD = texture(albedo_map, fs_uv);
	N = normalize(vec3(texture(normals_map, fs_uv)));
	float roughness = texture(roughness_map, fs_uv).x;

	vec4 F0 = vec4(0.06, 0.06, 0.06, 1.0);

	// Start from black
	fs_color = BLACK;

	// Compute each light's contribution
	for (int i = 0; i < MAX_NUM_LIGHTS; i++)
	{
		vec4 diffuse = BLACK;

		// Normalize the light input vectors as they have also been interpolated
		vec3 L = normalize(vs_lights.L[i]);
		vec3 H = normalize(vs_lights.H[i]);

		// Compute the light's dot products
		float NdotH = max(dot(N, H), 0.0);
		float NdotL = max(dot(N, L), 0.0);
		float NdotV = max(dot(N, V), 0.0);
		float VdotH = max(dot(V, H), 0.0);

		// Diffusive contribution
		diffuse = mix(Lambert(kD, vs_lights.kL[i], NdotL),
					  Lambert_NPR(kD, vs_lights.kL[i], dot(N, L),
								  lambertian.halfLambert,
								  lambertian.halfLambertBias,
								  lambertian.colorShades,
								  lambertian.blendedShadePercent,
								  lambertian.hsvShift),
					  lambertian.PBRtoNPR);

		// Add the light contribution
		fs_color += Attenuation(vs_lights.D[i]) * diffuse;
	}
}

float Attenuation(in float D)
{
	D = max(D, + KINDA_SMALL_NUMBER);
	return min(1.0 / (D * D), 1.0);
}

vec4 Lambert(in vec4 kD, in vec4 kL, in float NdotL)
{
	return vec4(vec3(kD) * vec3(kL) * NdotL, kD.a * kL.a);
}

vec4 HalfLambert(in vec4 kD, in vec4 kL, in float NdotL, in float exponent, in float bias)
{
	exponent = max(exponent, 0.0);

	NdotL = pow(NdotL * (1.0 - bias) + bias, exponent);

	return vec4(vec3(kD) * vec3(kL) * NdotL, kD.a * kL.a);
}

vec4 Lambert_NPR(in vec4 kD, in vec4 kL, in float NdotL, in bool halfLambert, in float bias, in int shades, in float blendPercent, in bool hsv)
{
	// Clamp the number of color shades
	shades = max(shades, 1);
	float fShades = float(shades);

	// Find the current shade index and the increment
	float increment = clamp(1.0 / fShades, 0.0, 1.0);
	float shadeIndex = float(floor(NdotL / increment));

	// Take the difference between half the increment and half the blend percent to find the range of the shade area,
	// centered in the middle of the shade band, which should be blended with the nearby shade bands
	// Divide the blend percent by 2 (to apply it on both sides of the current shade), scale it to the increment
	// and clamp it to half the increment (so that blending edges do not cross each other)
	blendPercent = max(((1.0 - blendPercent) / 2.0) * increment - VERY_SMALL_NUMBER, VERY_SMALL_NUMBER);

	// Find the NdotL range of the current shade
	float NdotLMin = clamp(shadeIndex * increment, 0.0, 1.0);
	float NdotLMax = clamp((shadeIndex + 1.0) * increment, 0.0, 1.0);

	// Add the blending range to the shade edges
	float shadeMin = clamp(NdotLMin + blendPercent, 0.0, 1.0);
	float shadeMax = clamp(NdotLMax - blendPercent, 0.0, 1.0);

	// Compute the shade blending factor within the current shade edges
	float shadeBlend = smoothstep(shadeMin, shadeMax, NdotL);

	// Adjust NdotL
	NdotL = mix(NdotLMin, NdotLMax, shadeBlend);

	// Find the color shift range of the current shade
	float shiftMin = clamp(fShades - shadeIndex, 0.0, fShades);
	float shiftMax = clamp(fShades - (shadeIndex + 1.0), 0.0, fShades - 1.0);

	// Define the kD RGB shift parameters
	float kDRgbDecrement = 0.1;
	float kDRgbShift = mix(shiftMin * kDRgbDecrement, shiftMax * kDRgbDecrement, shadeBlend);

	// Define the kD HSV shift parameters
	float kDHueDecrement = 0.06;
	float kDHueShift = mix(shiftMin * kDHueDecrement, shiftMax * kDHueDecrement, shadeBlend);
	float kDValueDecrement = 0.25 / fShades;
	float kDValueShift = mix(shiftMin * kDValueDecrement, shiftMax * kDValueDecrement, shadeBlend);

	// Define the kL RGB shift parameters
	//float kLRgbDecrement = 0.1;
	//float kLRgbShift = mix(shiftMin * kLRgbDecrement, shiftMax * kLRgbDecrement, shadeBlend);

	// Define the kL HSV shift parameters
	float kLHueDecrement = 0.5 / fShades;
	float kLHueShift = mix(shiftMin * kLHueDecrement, shiftMax * kLHueDecrement, shadeBlend);

	// Compute the shifted shade of kD
	kD = vec4(ShiftKD(vec3(kD), hsv, kDRgbShift, kDHueShift, kDValueShift), kD.a);
	// Compute the shifted shade of kL
	kL = vec4(ShiftKL(vec3(kL), hsv, kLHueShift), kL.a);

	// Convert the half-Lambert boolean to a float for the Half-Lambert function
	float fHalfLambert = clamp(float(halfLambert), 0.0, 1.0);

	// Compute the output color
	return HalfLambert(kD, kL, NdotL, fHalfLambert + 1.0, fHalfLambert * bias);
}

vec3 ShiftKD(in vec3 kD, in bool hsv, in float rgbShift, in float hueShift, in float valueShift)
{
	float fHSV = float(hsv);

	// If HSV is disabled, scale the RGB kD color,
	// otherwise compute a darker shade of kD
	
	// Hue: there is a larger shift as shading goes from light to shadow;
	//		the amount depends on the base color - a good default shift could be 12.5%;
	// Saturation: usually does not change;
	// Value: decreases linearly by roughly 25% from lightest color to darkest color;

	kD = mix(kD, RGBtoHSV(kD), fHSV); 

	kD = mix(kD - vec3(clamp(rgbShift, 0.0, 1.0)),
			 vec3(mod(kD.r - hueShift, 1.0), kD.g, max(kD.b - valueShift, 0.0)),
			 fHSV);

	kD = mix(kD, HSVtoRGB(kD), fHSV); 

	return kD;
}

vec3 ShiftKL(in vec3 kL, in bool hsv, in float hueShift)
{
	float fHSV = float(hsv);

	// If HSV is disabled, return the original RGB kL color,
	// otherwise compute the complementary of the HSV kL color

	kL = mix(kL, RGBtoHSV(kL), fHSV);

	kL = mix(kL,
			 vec3(mod(kL.r - hueShift, 1.0), kL.g, kL.b),
			 fHSV);
	
	kL = mix(kL, HSVtoRGB(kL), fHSV); 

	return kL;
}

vec3 RGBtoHSV(in vec3 RGB)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(RGB.bg, K.wz), vec4(RGB.gb, K.xy), step(RGB.b, RGB.g));
    vec4 q = mix(vec4(p.xyw, RGB.r), vec4(RGB.r, p.yzx), step(p.x, RGB.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;

    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 HSVtoRGB(vec3 HSV)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(HSV.xxx + K.xyz) * 6.0 - K.www);

    return HSV.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), HSV.y);
}