#pragma once

#include "interfaces/IModel.h"

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

		virtual glm::vec3 getPosition() const noexcept override;

		virtual void setPosition(const glm::vec3 newPosition) noexcept override;

		virtual glm::vec3 getRotation() const noexcept override;

		virtual void setRotation(const glm::vec3 newRotation) noexcept override;

		virtual glm::vec3 getScale() const noexcept override;

		virtual void setScale(const glm::vec3 newScale) noexcept override;

		virtual glm::mat4 getModelMatrix() const noexcept override;

		virtual std::shared_ptr<IShaderProgram> getProgram() const noexcept override;

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