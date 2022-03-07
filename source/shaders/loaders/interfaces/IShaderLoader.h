#pragma once

#include <string>
#include <glad/glad.h>

// The interface that Shader Loader classes must implement

class IShaderLoader
{
	public:
		virtual ~IShaderLoader() noexcept {};

		virtual bool load(std::string & source) noexcept = 0;

		virtual void unload() noexcept = 0;

	protected:
		IShaderLoader() {};

		// Disallowed - no need for two  instances of the same loader
		IShaderLoader(const IShaderLoader & copy) = delete;

		IShaderLoader & operator= (const IShaderLoader & copy) = delete;

		// Disallowed - no need to move a loader
		IShaderLoader(IShaderLoader && move) = delete;

		IShaderLoader & operator= (IShaderLoader && move) = delete;
};