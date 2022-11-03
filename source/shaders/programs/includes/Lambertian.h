#pragma once

#include <glad/glad.h>
#include <string>

#define LAMBERTIAN_BLOCK_NAME "Lambertian"
#define LAMBERTIAN_BINDING_INDEX (GLuint)2

// Lambertian parameters data structure

struct Lambertian
{
	// PBR / NPR shading interpolation
	float PBRtoNPR = 1.0;

	// Half-Lambert enabled / disabled
	bool halfLambert = true;

	// Half-Lambert bias
	float halfLambertBias = 0.5;

	// Number of distinct color shades
	int colorShades = 3;

	// Percentage of a shade area that is blended
	// with nearby shade areas
	float blendedShadePercent = 0.5f;

	// HSV shade shift enabled / disabled
	bool hsvShift = true;

	// Returns the uniform block name
	static const std::string getBlockName()
	{
		return LAMBERTIAN_BLOCK_NAME;
	}

	// Returns the uniform block binding index
	static GLuint getBindingIndex()
	{
		return LAMBERTIAN_BINDING_INDEX;
	}
};