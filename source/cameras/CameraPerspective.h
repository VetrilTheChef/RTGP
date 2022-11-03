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

		~CameraPerspective() noexcept;

		// Get the camera position
		virtual glm::vec3 getPosition() const noexcept override;

		// Set the camera position
		virtual void setPosition(const glm::vec3 newPosition) noexcept override;

		// Get the camera rotation
		virtual glm::vec3 getRotation() const noexcept override;

		// Set the camera rotation
		virtual void setRotation(const glm::vec3 newRotation) noexcept override;

		// Get the camera vertical FOV
		virtual float getVerticalFOV() const noexcept;

		// Set the camera vertical FOV
		virtual void setVerticalFOV(const float newFovY) noexcept;

		// Get the camera aspect ratio
		virtual float getAspectRatio() const noexcept;

		// Set the camera aspect ratio
		virtual void setAspectRatio(const float newAspectRatio) noexcept;

		// Get the camera near plane
		virtual float getNearPlane() const noexcept;

		// Set the camera near plane
		virtual void setNearPlane(const float newNearPlane) noexcept;

		// Get the camera far plane
		virtual float getFarPlane() const noexcept;

		// Set the camera far plane
		virtual void setFarPlane(const float newFarPlane) noexcept;

		// Set the camera look at target
		virtual void lookAt(const glm::vec3 newTarget, const glm::vec3 newUp)
			noexcept override;

		// Get the camera projection matrix
		virtual const glm::mat4 & getProjectionMatrix() const noexcept override;

		// Get the camera view matrix
		virtual const glm::mat4 & getViewMatrix() const noexcept override;

		// Get the camera view vector
		virtual const glm::vec3 & getViewVector() const noexcept override;

		// Get the camera forward vector
		virtual glm::vec3 forward() const noexcept override;

		// Get the camera right vector
		virtual glm::vec3 right() const noexcept override;

		// Get the camera up vector
		virtual glm::vec3 up() const noexcept override;

		// Update the camera
		virtual void update(double deltaSeconds) noexcept override;

		// Draw the camera object
		virtual void draw() noexcept override;

	protected:
		glm::vec3 position = glm::vec3(0.0f);

		glm::vec3 rotation = glm::vec3(0.0f);

		float fovY = 45.0f;
		
		float aspectRatio = 1.33f;

		float nearPlane = 0.0f;

		float farPlane = 10.0f;

		glm::mat4 projectionMatrix = glm::mat4();

		glm::mat4 viewMatrix = glm::mat4();

		glm::vec3 viewVector = glm::vec3();

		// Update the projection matrix
		void updateProjectionMatrix();

		// Update the view matrix
		void updateViewMatrix();

		// Update the view vector
		void updateViewVector();

	private:
		// Disallowed - provide the perspective parameters
		CameraPerspective() = delete;

		glm::mat4 orientation() const noexcept;
};