#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "lights/includes/Lights.h"
#include "shaders/loaders/interfaces/IShaderLoader.h"
#include "shaders/programs/includes/MVPN.h"

// The interface that Shader Program classes must implement

class IShaderProgram
{
	public:
		virtual ~IShaderProgram() noexcept {};

		virtual void activate() noexcept = 0;

		virtual void deactivate() const noexcept = 0;

		virtual void setViewVector(const GLfloat * newViewVector) noexcept = 0;

	protected:
		// Disallowed - must provide shader loaders, mvp and lights
		IShaderProgram() = delete;

		IShaderProgram(std::shared_ptr<IShaderLoader> newVertexShaderLoader,
					   std::shared_ptr<IShaderLoader> newFragmentShaderLoader,
					   const MVPN & newMvpn,
					   const Lights & newLights)
					   noexcept {};

		// Disallowed - no need for 2 instances of the same shader program
		IShaderProgram(const IShaderProgram & copy) = delete;
		IShaderProgram & operator= (const IShaderProgram & copy) = delete;

		// Disallowed - no need to move a fragment shader
		IShaderProgram(IShaderProgram && move) = delete;
		IShaderProgram & operator= (IShaderProgram && move) = delete;
};