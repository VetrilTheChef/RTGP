#include "UniformBufferObject.h"
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

UniformBufferObject::UniformBufferObject() noexcept :
	IUniformBufferObject()
{
}

UniformBufferObject::~UniformBufferObject() noexcept
{
	destroy();
}

void UniformBufferObject::create(GLsizeiptr newSize) noexcept
{
	size = newSize;

	glGenBuffers(1, & UBO);
}

void UniformBufferObject::bind(GLuint newProgram,
							   const string & newBlockName,
							   GLuint newIndex) noexcept
{
	index = newIndex;

	// Get the Uniform Block Index
	GLuint UBI = glGetUniformBlockIndex(newProgram, newBlockName.c_str());

	// If the block is found
	if (UBI != GL_INVALID_INDEX)
	{
		// Done by the layout qualifier in the GLSL shader
		//glUniformBlockBinding(newProgram, UBI, index);

		// Initialize with NULL data
		glBindBufferBase(GL_UNIFORM_BUFFER, index, UBO);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	else
	{
		// Log a warning
		cout << "Uniform Buffer Object: could not find block named "
			<< newBlockName << "." << endl;
	}
}

void UniformBufferObject::update(const void * newData) noexcept
{
	// Pass the new data to the buffer
	glBindBufferBase(GL_UNIFORM_BUFFER, index, UBO);
	glBufferData(GL_UNIFORM_BUFFER, size, newData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBufferObject::destroy() noexcept
{
	glDeleteBuffers(1, & UBO);
}
