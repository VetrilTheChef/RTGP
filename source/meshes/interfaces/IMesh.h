#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include "meshes/includes/Vertex.h"

// The interface that Mesh classes must implement

class IMesh
{
	public:
		virtual ~IMesh() noexcept {};

		// The mesh file path
		std::string path;

		GLuint VAO = 0;

		GLuint VBO = 0;

		GLuint EBO = 0;

		// The mesh vertex data
		std::vector<Vertex> vertices;

		// The mesh index data
		std::vector<GLuint> indices;

		// Draw the mesh on screen
		virtual void draw() const noexcept = 0;

	protected:
		// Disallowed - must provide a mesh path
		IMesh() = delete;

		IMesh(std::string & newPath) noexcept :
			path(newPath) {};

		IMesh(std::string && newPath) noexcept :
			path(newPath) {};

		IMesh(const IMesh & copy) noexcept {};

		IMesh & operator= (const IMesh & copy) noexcept {};

		IMesh(IMesh && move) noexcept {};

		IMesh & operator= (IMesh && move) noexcept {};
};