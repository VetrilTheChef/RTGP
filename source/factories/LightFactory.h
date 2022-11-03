#pragma once

#include "interfaces/ILightFactory.h"

// This class represents a light factory.
// It is responsible for creating and initializing all types of lights.

class LightFactory : public ILightFactory
{
	public:
		LightFactory() noexcept;

		~LightFactory() noexcept;

		// Create a point light
		virtual bool createPointLight(std::shared_ptr<ILight> & light,
									  glm::vec3 position,
									  glm::vec3 color,
									  float intensity) const noexcept override;
};