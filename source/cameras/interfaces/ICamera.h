#pragma once

#include <glm/glm.hpp>

// The interface that Camera classes must implement

class ICamera
{
	public:
		virtual ~ICamera() noexcept {};

		// Get the camera position
		virtual glm::vec3 getPosition() const noexcept = 0;

		// Set the camera position
		virtual void setPosition(const glm::vec3 newPosition) noexcept = 0;

		// Get the camera rotation
		virtual glm::vec3 getRotation() const noexcept = 0;

		// Set the camera rotation
		virtual void setRotation(const glm::vec3 newRotation) noexcept = 0;

		// Set the camera look at target
		virtual void lookAt(const glm::vec3 newTarget,
							const glm::vec3 newUp) noexcept = 0;

		// Get the camera projection matrix
		virtual const glm::mat4 & getProjectionMatrix() const noexcept = 0;

		// Get the camera view matrix
		virtual const glm::mat4 & getViewMatrix() const noexcept = 0;

		// Get the camera view vector
		virtual const glm::vec3 & getViewVector() const noexcept = 0;

		// Get the camera forward vector
		virtual glm::vec3 forward() const noexcept = 0;

		// Get the camera right vector
		virtual glm::vec3 right() const noexcept = 0;

		// Get the camera up vector
		virtual glm::vec3 up() const noexcept = 0;

		// Update the camera
		virtual void update(double deltaSeconds) noexcept = 0;

		// Draw the camera object
		virtual void draw() noexcept = 0;

	protected:
		ICamera() noexcept {};

		// As cameras can be very different, require explicit construction
		// so that parameters must be provided

		ICamera(const ICamera & copy) = delete;
		ICamera & operator= (const ICamera & copy) = delete;

		ICamera(ICamera && move) = delete;
		ICamera & operator= (ICamera && move) = delete;
};