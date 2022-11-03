#pragma once

#include <glad/glad.h>
#include <string>

// The interface that Texture classes must implement

class ITexture
{
	public:
		virtual ~ITexture() noexcept {};

		// Create the texture
		virtual bool create() noexcept = 0;

		// Bind the program's block to the requested index
		virtual void bind(GLuint newProgram, const std::string & newBlockName,
						  GLuint newIndex) noexcept = 0;

		// Activate the texture
		virtual void activate() noexcept = 0;

		// Deactivate the texture
		virtual void deactivate() noexcept = 0;

		// Destroy the texture
		virtual void destroy() noexcept = 0;

	protected:
		ITexture() {};

		// Disallowed - no need for 2 instances of the same texture
		ITexture(const ITexture& copy) = delete;
		ITexture& operator= (const ITexture& copy) = delete;

		// Disallowed - no need to move a texture
		ITexture(ITexture&& move) = delete;
		ITexture& operator= (ITexture&& move) = delete;
};