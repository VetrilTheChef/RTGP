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

		virtual glm::vec3 getPosition() const noexcept = 0;

		virtual void setPosition(const glm::vec3 newPosition) noexcept = 0;

		virtual glm::vec3 getRotation() const noexcept = 0;

		virtual void setRotation(const glm::vec3 newRotation) noexcept = 0;

		virtual glm::vec3 getScale() const noexcept = 0;

		virtual void setScale(const glm::vec3 newScale) noexcept = 0;

		virtual glm::mat4 getModelMatrix() const noexcept = 0;

		virtual std::shared_ptr<IShaderProgram> getProgram() const noexcept = 0;

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