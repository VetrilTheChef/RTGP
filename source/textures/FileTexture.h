#pragma once

#include "interfaces/ITexture.h"
#include <glm/glm.hpp>

class FileTexture : public ITexture
{
	public:
		FileTexture(std::string& newTexturePath) noexcept;

		FileTexture(std::string&& newTexturePath) noexcept;

		~FileTexture() noexcept;

		// Create the texture
		virtual bool create() noexcept override;

		// Bind the program's block to the requested index
		virtual void bind(GLuint newProgram, const std::string & newBlockName,
						  GLuint newIndex) noexcept override;

		// Activate the texture
		virtual void activate() noexcept override;

		// Deactivate the texture
		virtual void deactivate() noexcept override;

		// Destroy the texture
		virtual void destroy() noexcept override;

	private:
		// The texture file path
		std::string path = "";

		// The texture id
		GLuint texture = -1;

		// The texture width
		int width = 0;

		// The texture height
		int height = 0;
};