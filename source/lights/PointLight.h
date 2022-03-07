#pragma once

#include "interfaces/ILight.h"

// This class represents a point light.
// It stores information about its position, color, and intensity.

class PointLight : public ILight
{
	public:
		PointLight(glm::vec3 newPosition,
				   glm::vec3 newColor,
				   float newIntensity) noexcept;

		PointLight(const PointLight & copy) noexcept;

		ILight & operator= (const ILight & copy) noexcept;

		PointLight(ILight && move) noexcept;

		ILight & operator= (ILight && move) noexcept;

		~PointLight() noexcept;

		virtual glm::vec3 getPosition() const noexcept override;

		virtual void setPosition(const glm::vec3 newPosition) noexcept override;

		virtual float getIntensity() const noexcept override;

		virtual void setIntensity(const float newIntensity) noexcept override;

		virtual glm::vec3 getColor() const noexcept override;

		virtual void setColor(const glm::vec3 newColor) noexcept override;

		virtual void draw() noexcept override;

	protected:
		glm::vec3 position = glm::vec3(0.0f);

		// This packs the intensity in the w component
		glm::vec4 color = glm::vec4(0.0f);

	private:
		// Disallowed - provide the light parameters
		PointLight() = delete;
};