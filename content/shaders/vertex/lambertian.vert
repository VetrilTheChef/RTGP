#version 430 core

#define MAX_NUM_LIGHTS 4
#define PI 3.1415926535

uniform vec3 view;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 uv;

layout(std140, binding = 0) uniform MVPN
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 normal;
} mvpn;

layout(std140, binding = 1) uniform Lights
{
	vec4 positions[MAX_NUM_LIGHTS];
	vec4 colors[MAX_NUM_LIGHTS];
} lights;

out vec3 vs_P;
out vec3 vs_N;
out vec3 vs_V;
out vec2 vs_uv;

out VS_LIGHTS
{
	vec3 L[MAX_NUM_LIGHTS];
	vec3 H[MAX_NUM_LIGHTS];
	vec4 kL[MAX_NUM_LIGHTS];
	float D[MAX_NUM_LIGHTS];
} vs_lights;

void main()
{
	// The fragment shader will output all vectors in tangent space.
	// This is an optimization as the vertex shader is executed less
	// frequently than the fragment shader, so keeping tangent space
	// calculations here will lead to less overall calculations.
	// As a downside, since the vectors will be interpolated, they
	// might no longer be normalized once they are passed to the 
	// fragment shader.

	// We don't output the dot products here as some of them are
	// per-light and, as the number of lights increases, the vertex
	// shader won't be able to output enough values to handle all of
	// them.

	// Tangent space matrix
	vec3 T = normalize(vec3(mvpn.model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(mvpn.model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(mvpn.model * vec4(normal, 0.0)));
	mat3 TBN = transpose(mat3(T, B, N));
	
	// Fragment position
	vs_P = TBN * vec3(mvpn.model * vec4(position, 1.0));

	// Fragment normal
	vs_N = normalize(TBN * vec3(mvpn.normal * vec4(normal, 0.0)));

	// Fragment texture coordinates
	vs_uv = uv;
	
	// View direction
	vs_V = normalize(TBN * view);

	for (int i = 0; i < MAX_NUM_LIGHTS; i++)
	{
		// Compute each light direction vs_lights.positions[i]
		vec3 L = vec3(lights.positions[i]) - vec3(mvpn.model * vec4(position, 1.0));
		vs_lights.L[i] = normalize(TBN * L);

		// Parallelogram Law says H's direction can be found by adding L and V
		vs_lights.H[i] = normalize(vs_lights.L[i] + vs_V);

		// Compute each light color vs_lights.colors[i]
		// Reminder: w is the light's intensity
		vs_lights.kL[i] = vec4(vec3(lights.colors[i]) * lights.colors[i].w, 1.0);

		// Light distance
		vs_lights.D[i] = max(distance((TBN * vec3(lights.positions[i])), vs_P), 0.01);
	}

	// Compute the output position
	gl_Position = mvpn.projection * mvpn.view * mvpn.model * vec4(position, 1.0);
}