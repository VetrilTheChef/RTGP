#pragma once

#include <memory>
#include "interfaces/IShaderProgram.h"
#include "shaders/buffers/interfaces/IUniformBufferObject.h"

class ShaderProgram : public IShaderProgram
{
	public:
		ShaderProgram(std::shared_ptr<IShaderLoader> newVertexShaderLoader,
					  std::shared_ptr<IShaderLoader> newFragmentShaderLoader,
					  const MVPN & newMvpn,
					  const Lights & newLights)
			noexcept;

		~ShaderProgram() noexcept;

		virtual void activate()
			noexcept override;

		virtual void deactivate() const noexcept override;

		virtual void setViewVector(const GLfloat * newViewVector) noexcept override;

	protected:
		std::shared_ptr<IShaderLoader> vertexShaderLoader;

		std::shared_ptr<IShaderLoader> fragmentShaderLoader;

		GLint id = 0;

		const MVPN * mvpn = nullptr;

		const Lights * lights = nullptr;

		std::unique_ptr<IUniformBufferObject> mvpnUBO;

		std::unique_ptr<IUniformBufferObject> lightsUBO;

		// Create the shaders and initialize them
		bool createShaders(GLint & vertexShaderId,
						   GLint & fragmentShaderId) noexcept;

		// Delete the shaders
		void deleteShaders(GLint vertexShaderId,
						   GLint fragmentShaderId) noexcept;

		// Create the shader program
		bool createProgram(GLint vertexShaderId,
						   GLint fragmentShaderId) noexcept;

		// Delete the shader program
		void deleteProgram() noexcept;

		// Create and bind the uniform block object buffers
		void createUBOs() noexcept;

		// Delete the uniform block object buffers
		void deleteUBOs() noexcept;
};