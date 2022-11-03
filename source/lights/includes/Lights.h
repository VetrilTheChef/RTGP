#pragma once

#include <glad/glad.h>
#include <string>
#include "glm/glm.hpp"

#define LIGHTS_BLOCK_NAME "Lights"
#define LIGHTS_BINDING_INDEX (GLuint)1
#define LIGHTS_MAX_LIGHTS 4

// Light data structure

enum LightType { None, Point };

struct Lights
{
	// GLSL std140 layout
	// MEMBER       TYPE     STRIDE  
	// Positions    vec3[]   4N
	// Colors       vec4[]   4N

	// Arrays have element aligned to a multiple of 4N
	// This alignment must be enforced on the application side
	// Thus, alignas(sizeof(float) * 4)

	// ...Except it doesn't work, so now Positions is an array of vec4
	 
	// Position
	glm::vec4 Positions[LIGHTS_MAX_LIGHTS] = { glm::vec4(0.0f) };

	// Color (Linear) - W component is the light's intensity
	glm::vec4 Colors[LIGHTS_MAX_LIGHTS] = { glm::vec4(0.0f) };

	// Returns the uniform block name
	static const std::string getBlockName()
	{
		return LIGHTS_BLOCK_NAME;
	}

	// Returns the uniform block binding index
	static GLuint getBindingIndex()
	{
		return LIGHTS_BINDING_INDEX;
	}

	// Returns the maximum number of lights
	static unsigned int getMaxLights()
	{
		return LIGHTS_MAX_LIGHTS;
	}
};

struct Light
{
	// GLSL std140 layout
	// MEMBER       TYPE     STRIDE 
	// Type         int 
	// Position     vec3     4N
	// Color        vec4     4N

	// Arrays have element aligned to a multiple of 4N
	// This alignment must be enforced on the application side
	// Thus, alignas(sizeof(float) * 4)

	// ...Except it doesn't work, so now Positions is an array of vec4

	// Type
	LightType Type = { LightType::None };

	// Position
	glm::vec4 Position = { glm::vec4(0.0f) };

	// Color (Linear) - W component is the light's intensity
	glm::vec4 Color = { glm::vec4(0.0f) };

	// Returns the uniform block name
	static const std::string getBlockName()
	{
		return LIGHTS_BLOCK_NAME;
	}

	// Returns the uniform block binding index
	static GLuint getBindingIndex()
	{
		return LIGHTS_BINDING_INDEX;
	}

	// Returns the maximum number of lights
	static unsigned int getMaxLights()
	{
		return LIGHTS_MAX_LIGHTS;
	}
};