#pragma once

#include "interfaces/IShaderLoader.h"

class FileShaderLoader : public IShaderLoader
{
	public:
		FileShaderLoader(std::string & newPath) noexcept;

		FileShaderLoader(std::string && newPath) noexcept;

		virtual bool load(std::string & source) noexcept;

		virtual void unload() noexcept;

		~FileShaderLoader() noexcept;

	private:
		// The shader file path
		std::string path = "";

		// The vertex shader source
		std::string source = "";

		// Create a default shader source as fallback
		void createDefault();
};