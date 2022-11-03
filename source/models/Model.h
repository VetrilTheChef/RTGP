#pragma once

#include "interfaces/IModel.h"

// This class represents a model composed of a mesh and a shader program.
// It is responsible for handling transformations, and rendering its
// components using OpenGL.

class Model : public IModel
{
	public:
		Model(std::unique_ptr<IMesh> newMesh,
			  std::shared_ptr<IShaderProgram> newProgram) noexcept;

		Model(std::unique_ptr<IMesh> newMesh,
			  std::shared_ptr<IShaderProgram> newProgram,
			  glm::vec3 newPosition,
			  glm::vec3 newRotation,
			  glm::vec3 newScale) noexcept;

		Model(IModel && move) noexcept;

		IModel & operator= (IModel && move) noexcept;

		// Get the model position
		virtual glm::vec3 getPosition() const noexcept override;

		// Set the model position
		virtual void setPosition(const glm::vec3 newPosition) noexcept override;

		// Get the model rotation
		virtual glm::vec3 getRotation() const noexcept override;

		// Set the model rotation
		virtual void setRotation(const glm::vec3 newRotation) noexcept override;

		// Get the model scale
		virtual glm::vec3 getScale() const noexcept override;

		// Set the model scale
		virtual void setScale(const glm::vec3 newScale) noexcept override;

		// Get the model matrix
		virtual glm::mat4 getModelMatrix() const noexcept override;

		// Get the model program
		virtual std::shared_ptr<IShaderProgram> getProgram() const noexcept override;

		// Update the model
		virtual void update(double deltaSeconds) noexcept override;

		// Render the model
		void render() const noexcept override;

		~Model() noexcept;

	protected:
		// The model's position
		glm::vec3 position = glm::vec3(0.0f);

		// The model's rotation
		glm::vec3 rotation = glm::vec3(0.0f);

		// The model's scale
		glm::vec3 scale = glm::vec3(1.0f);

		// The model matrix
		glm::mat4 modelMatrix = glm::mat4();

		void updateModelMatrix() noexcept;
};