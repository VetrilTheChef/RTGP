#pragma once

#include <glad/glad.h>
#include <string>

// The interface that Uniform Buffer Object classes must implement

class IUniformBufferObject
{
	public:
		virtual ~IUniformBufferObject() noexcept {};

		// Create the UBO's buffer
		virtual void create(GLsizeiptr newSize) noexcept = 0;

		// Bind the program's block to the requested index
		virtual void bind(GLuint newProgram,
						  const std::string & newBlockName,
						  GLuint newIndex) noexcept = 0;

		// Updates the buffer data
		virtual void update(const void * newData) noexcept = 0;

		// Destroy the UBO
		virtual void destroy() noexcept = 0;

	protected:
		IUniformBufferObject() {};

		// Disallowed - no need for 2 instances of the same UBO
		IUniformBufferObject(const IUniformBufferObject & copy) = delete;
		IUniformBufferObject& operator= (const IUniformBufferObject & copy) = delete;

		// Disallowed - no need to move a UBO
		IUniformBufferObject(IUniformBufferObject && move) = delete;
		IUniformBufferObject& operator= (IUniformBufferObject && move) = delete;
};