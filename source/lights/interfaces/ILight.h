#pragma once

#include <glm/glm.hpp>

// The interface that Light classes must implement

class ILight
{
	public:
		virtual ~ILight() noexcept {};

		// Get the light position
		virtual glm::vec3 getPosition() const noexcept = 0;

		// Set the light position
		virtual void setPosition(const glm::vec3 newPosition) noexcept = 0;

		// Get the light intensity
		virtual float getIntensity() const noexcept = 0;

		// Set the light intensity
		virtual void setIntensity(const float newIntensity) noexcept = 0;

		// Get the light color (SRGB)
		virtual glm::vec3 getColor() const noexcept = 0;

		// Set the light color (SRGB)
		virtual void setColor(const glm::vec3 newColor) noexcept = 0;

		// Update the light
		virtual void update(double deltaSeconds) noexcept = 0;

		// Draw the light object
		virtual void draw() noexcept = 0;

	protected:
		// Disallowed - must provide position, intensity, color
		ILight() = delete;

		ILight(glm::vec3 newPosition,
			   glm::vec3 newColor,
			   float newIntensity) noexcept {};

		ILight(const ILight & copy) noexcept {};

		ILight & operator= (const ILight & copy) noexcept {};

		ILight(ILight && move) noexcept {};

		ILight & operator= (ILight && move) noexcept {};
};