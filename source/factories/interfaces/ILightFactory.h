#pragma once

#include <glm/glm.hpp>
#include <memory>

// Forward declarations

class ILight;

// The interface that Light Factory classes must implement

class ILightFactory
{
	public:
		virtual ~ILightFactory() noexcept {};

		// Create a point light
		virtual bool createPointLight(std::shared_ptr<ILight> & light,
									  glm::vec3 position,
									  glm::vec3 color,
									  float intensity) const noexcept = 0;

	protected:
		ILightFactory() {};

		// Disallowed - no need for 2 instances of the same light factory
		ILightFactory(const ILightFactory & copy) = delete;
		ILightFactory& operator= (const ILightFactory & copy) = delete;

		// Disallowed - no need to move a light factory
		ILightFactory(ILightFactory && move) = delete;
		ILightFactory& operator= (ILightFactory && move) = delete;
};