#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "lights/includes/Lights.h"
#include "meshes/interfaces/IMesh.h"
#include "shaders/programs/interfaces/IShaderProgram.h"

// The interface that Model classes must implement

class IModel
{
	public:
		// The model's mesh
		std::unique_ptr<IMesh> mesh;

		// The model's shader program
		std::shared_ptr<IShaderProgram> program;

		// Get the model position
		virtual glm::vec3 getPosition() const noexcept = 0;

		// Set the model position
		virtual void setPosition(const glm::vec3 newPosition) noexcept = 0;

		// Get the model rotation
		virtual glm::vec3 getRotation() const noexcept = 0;

		// Set the model rotation
		virtual void setRotation(const glm::vec3 newRotation) noexcept = 0;

		// Get the model scale
		virtual glm::vec3 getScale() const noexcept = 0;

		// Set the model scale
		virtual void setScale(const glm::vec3 newScale) noexcept = 0;

		// Get the model matrix
		virtual glm::mat4 getModelMatrix() const noexcept = 0;

		// Get the model shader program
		virtual std::shared_ptr<IShaderProgram> getProgram() const noexcept = 0;

		// Update the model
		virtual void update(double deltaSeconds) noexcept = 0;

		// Render the model
		virtual void render() const noexcept = 0;

	protected:
		// Disallowed - must provide at least a mesh and a program
		IModel() = delete;

		IModel(std::unique_ptr<IMesh> newMesh,
			std::shared_ptr<IShaderProgram> newProgram) noexcept :
			mesh(std::move(newMesh)),
			program(std::move(newProgram)) {};

		IModel(const IModel & copy) noexcept {};

		IModel & operator= (const IModel & copy) noexcept {};

		IModel(IModel && move) noexcept {};

		IModel & operator= (IModel && move) noexcept {};

		virtual ~IModel() noexcept {};
};