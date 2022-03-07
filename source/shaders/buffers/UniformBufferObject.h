#pragma once

#include "interfaces/IUniformBufferObject.h"

class UniformBufferObject : public IUniformBufferObject
{
	public:
		UniformBufferObject() noexcept;

		~UniformBufferObject() noexcept;

		virtual void create(GLsizeiptr newSize) noexcept override;

		virtual void bind(GLuint newProgram,
						  const std::string & newBlockName,
						  GLuint newIndex) noexcept override;

		virtual void update(const void * newData) noexcept override;

		virtual void destroy() noexcept override;

	private:
		// The UBO handle
		GLuint UBO = 0;

		// The buffer size
		GLsizeiptr size = 0;

		// The block binding index
		GLuint index = 0;
};