#pragma once

#include <glm/glm.hpp>

// The interface that Camera classes must implement

class ICamera
{
	public:
		virtual ~ICamera() noexcept {};

		virtual glm::vec3 getPosition() const noexcept = 0;

		virtual void setPosition(const glm::vec3 newPosition) noexcept = 0;

		virtual glm::vec3 getRotation() const noexcept = 0;

		virtual void setRotation(const glm::vec3 newRotation) noexcept = 0;

		virtual void lookAt(const glm::vec3 newTarget,
							const glm::vec3 newUp) noexcept = 0;

		virtual glm::mat4 getProjectionMatrix() const noexcept = 0;

		virtual glm::mat4 getViewMatrix() const noexcept = 0;

		virtual glm::vec3 getViewVector() const noexcept = 0;

		virtual glm::vec3 forward() const noexcept = 0;

		virtual glm::vec3 right() const noexcept = 0;

		virtual glm::vec3 up() const noexcept = 0;

	protected:
		ICamera() noexcept {};

		// As cameras can be very different, require explicit construction
		// so that parameters must be provided

		ICamera(const ICamera & copy) = delete;

		ICamera & operator= (const ICamera & copy) = delete;

		ICamera(ICamera && move) = delete;

		ICamera & operator= (ICamera && move) = delete;
};