#pragma once

#include <glad/glad.h>
#include "glm/glm.hpp"

#define MVPN_BLOCK_NAME "MVPN"
#define MVPN_BINDING_INDEX (GLuint)0

// MVPN matrices data structure

struct MVPN
{
	// Model matrix
	glm::mat4 model{};

	// View matrix
	glm::mat4 view{};

	// Projection matrix
	glm::mat4 projection{};

	// Normal matrix
	glm::mat4 normal{};

	// Returns the uniform block name
	static const std::string getBlockName()
	{
		return MVPN_BLOCK_NAME;
	}

	// Returns the uniform block binding index
	static GLuint getBindingIndex()
	{
		return MVPN_BINDING_INDEX;
	}
};