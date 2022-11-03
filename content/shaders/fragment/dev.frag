#version 430 core

#define MAX_NUM_LIGHTS 4
#define PI 3.1415926535
#define BLACK vec4(0.0, 0.0, 0.0, 1.0)
#define kindaSmallNumber (1.0 / 256.0)

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

float Attenuation(in float D);

vec4 Lambert(in vec4 kD, in vec4 kL, in float NdotL);

vec4 Fresnel_Schlick(in vec4 F0, in float VdotH);

float GeometryAttenuation_GGX(in float dotValue, in float k);

float GeometryAttenuation_Smith(in float roughness, in float NdotV, in float NdotL);

float NormalDistribution_GGX(in float roughness, in float NdotH);

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
		vec4 specular = BLACK;
		
		// Normalize the light input vectors as they have also been interpolated
		vec3 L = normalize(vs_lights.L[i]);
		vec3 H = normalize(vs_lights.H[i]);

		// Compute the light's dot products
		float NdotH = max(dot(N, H), 0.0);
		float NdotL = max(dot(N, L), 0.0);
		float NdotV = max(dot(N, V), 0.0);
		float VdotH = max(dot(V, H), 0.0);

		// Diffusive contribution
		diffuse = Lambert_NPR(kD, vs_lights.kL[i], NdotL, 4);

		// Compute the specular contribution only if the surface is facing the
		// light source
		if (NdotL > 0.0)
		{
			// Lighting equation terms
			vec4 F = Fresnel_Schlick(F0, VdotH);
			float D = NormalDistribution_GGX(roughness, NdotH);
			float G = GeometryAttenuation_Smith(roughness, NdotV, NdotL);

			// Compute the specular
			specular = (F * D * G) / (4.0 * NdotV * NdotL + kindaSmallNumber);
		}

		// Add the light contribution
		fs_color += Attenuation(vs_lights.D[i]) * vs_lights.kL[i] * (diffuse + specular) * NdotL;
	}
}

float Attenuation(in float D)
{
	D = max(D, + kindaSmallNumber);
	return min(1.0 / (D * D), 1.0);
}

vec4 Lambert(in vec4 kD, in vec4 kL, in float NdotL)
{
	return kD * kL * NdotL;
}

vec4 Fresnel_Schlick(in vec4 F0, in float VdotH)
{
	return F0 + (1.0 - F0) * vec4(vec3(pow(1.0 - VdotH, 5.0)), 1.0);
}

float GeometryAttenuation_GGX(in float roughness, in float angle)
{
	float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;

	float nom = angle;
	float denom = angle * (1.0 - k) + k;
	
	return nom / denom;
}

float GeometryAttenuation_Smith(in float roughness, in float NdotV, in float NdotL)
{
	return GeometryAttenuation_GGX(NdotV, roughness) * GeometryAttenuation_GGX(NdotL, roughness);
}

float NormalDistribution_GGX(in float roughness, in float NdotH)
{
	float roughness2 = roughness * roughness;
	float NdotH2 = NdotH * NdotH;
	float denom = NdotH2 * (roughness2 - 1.0) + 1.0;
	denom = PI * denom * denom;

	return (roughness2 / denom);
}