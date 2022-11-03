#pragma once

#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

// Vertex data structure

struct Vertex {
	// Coordinates
	glm::vec3 Position {};

	// Normal
	glm::vec3 Normal {};

	// Tangent
	glm::vec3 Tangent {};

	// Bitangent
	glm::vec3 Bitangent {};

	// UVs
	glm::vec2 TexCoords {};
};