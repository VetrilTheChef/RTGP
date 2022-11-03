#pragma once

#include "interfaces/IShaderProgram.h"
#include <memory>
#include "shaders/loaders/interfaces/IShaderLoader.h"
#include "shaders/buffers/interfaces/IUniformBufferObject.h"

// Macros to define shader uniforms block names and binding indices

#define ALBEDO_TEXTURE_INDEX (GLuint)0
#define NORMALS_TEXTURE_INDEX (GLuint)1
#define ROUGHNESS_TEXTURE_INDEX (GLuint)2

class ShaderProgram : public IShaderProgram
{
	public:
		ShaderProgram(std::shared_ptr<IShaderLoader> newVertexShaderLoader,
					  std::shared_ptr<IShaderLoader> newFragmentShaderLoader,
					  std::shared_ptr<ITexture> newAlbedoMap,
					  std::shared_ptr<ITexture> newNormalsMap,
					  std::shared_ptr<ITexture> newRoughnessMap,
					  const MVPN & newMvpn,
					  const Lights & newLights,
					  const Lambertian & newLambertian)
			noexcept;

		~ShaderProgram() noexcept;

		virtual void activate()
			noexcept override;

		virtual void deactivate() const noexcept override;

		virtual void setViewVector(const GLfloat * newViewVector) noexcept override;

	protected:
		// The loader responsible for the vertex shader
		std::shared_ptr<IShaderLoader> vertexShaderLoader;

		// The loader responsible for the fragment shader
		std::shared_ptr<IShaderLoader> fragmentShaderLoader;

		// The albedo texture
		std::shared_ptr<ITexture> albedoMap;

		// The normals texture
		std::shared_ptr<ITexture> normalsMap;

		// The roughness texture
		std::shared_ptr<ITexture> roughnessMap;

		// The shader program OpenGL id
		GLint id = 0;

		// The MVPN struct to pass to the shaders
		const MVPN * mvpn = nullptr;

		// The Lights struct to pass to the shaders
		const Lights * lights = nullptr;

		// The Lambertian struct to pass to the shaders
		const Lambertian * lambertian = nullptr;

		// The UBO responsible for the MVPN struct
		std::unique_ptr<IUniformBufferObject> mvpnUBO;

		// The UBO responsible for the Lights struct
		std::unique_ptr<IUniformBufferObject> lightsUBO;

		// The UBO responsible for the Lambertian struct
		std::unique_ptr<IUniformBufferObject> lambertianUBO;

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

		// Create and bind the shader program textures
		void createTextures() noexcept;

		// Delete the shader program textures
		void deleteTextures() noexcept;
};