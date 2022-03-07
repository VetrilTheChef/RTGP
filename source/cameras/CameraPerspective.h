#pragma once

#include "interfaces/ICamera.h"

// This class represents a camera featuring 3D perspective.
// It stores information used to generate both a view matrix,
// and a (perspective) projection matrix.

class CameraPerspective : public ICamera
{
	public:
		CameraPerspective(glm::vec3 newPosition,
						  glm::vec3 newRotation,
						  float newFovY,
						  float newAspectRatio,
						  float newNearPlane,
						  float newFarPlane) noexcept;

		virtual glm::vec3 getPosition() const noexcept override;

		virtual void setPosition(const glm::vec3 newPosition) noexcept override;

		virtual glm::vec3 getRotation() const noexcept override;

		virtual void setRotation(const glm::vec3 newRotation) noexcept override;

		virtual float getVerticalFOV() const noexcept;

		virtual void setVerticalFOV(const float newFovY) noexcept;

		virtual float getAspectRatio() const noexcept;

		virtual void setAspectRatio(const float newAspectRatio) noexcept;

		virtual float getNearPlane() const noexcept;

		virtual void setNearPlane(const float newNearPlane) noexcept;

		virtual float getFarPlane() const noexcept;

		virtual void setFarPlane(const float newFarPlane) noexcept;

		virtual void lookAt(const glm::vec3 newTarget,
							const glm::vec3 newUp) noexcept override;

		virtual glm::mat4 getProjectionMatrix() const noexcept override;

		virtual glm::mat4 getViewMatrix() const noexcept override;

		virtual glm::vec3 getViewVector() const noexcept override;

		virtual glm::vec3 forward() const noexcept override;

		virtual glm::vec3 right() const noexcept override;

		virtual glm::vec3 up() const noexcept override;

		~CameraPerspective() noexcept;

	protected:
		glm::vec3 position = glm::vec3(0.0f);

		glm::vec3 rotation = glm::vec3(0.0f);

		float fovY = 45.0f;
		
		float aspectRatio = 1.33f;

		float nearPlane = 0.0f;

		float farPlane = 10.0f;

		glm::mat4 projectionMatrix = glm::mat4();

		glm::mat4 viewMatrix = glm::mat4();

		// Update the projection matrix
		void updateProjectionMatrix();

		// Update the view matrix
		void updateViewMatrix();

	private:
		// Disallowed - provide the perspective parameters
		CameraPerspective() = delete;

		glm::mat4 orientation() const noexcept;
};