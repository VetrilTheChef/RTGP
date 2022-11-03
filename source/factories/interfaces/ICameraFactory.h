#pragma once

#include <glm/glm.hpp>
#include <memory>

// Forward declarations

class ICamera;

// The interface that Camera Factory classes must implement

class ICameraFactory
{
	public:
		virtual ~ICameraFactory() noexcept {};

		// Create a perspective camera
		virtual bool createPerspectiveCamera(std::unique_ptr<ICamera> & camera,
											 glm::vec3 position,
											 glm::vec3 rotation,
											 float fovY,
											 float aspectRatio,
											 float nearPlane,
											 float farPlane) const noexcept = 0;

	protected:
		ICameraFactory() {};

		// Disallowed - no need for 2 instances of the same camera factory
		ICameraFactory(const ICameraFactory & copy) = delete;
		ICameraFactory & operator= (const ICameraFactory & copy) = delete;

		// Disallowed - no need to move a camera factory
		ICameraFactory(ICameraFactory && move) = delete;
		ICameraFactory & operator= (ICameraFactory && move) = delete;
};