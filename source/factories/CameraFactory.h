#pragma once

#include "interfaces/ICameraFactory.h"

// This class represents a camera factory.
// It is responsible for creating and initializing all types of cameras.

class CameraFactory : public ICameraFactory
{
public:
	CameraFactory() noexcept;

	~CameraFactory() noexcept;

	// Create a perspective camera
	virtual bool createPerspectiveCamera(std::unique_ptr<ICamera> & camera,
										 glm::vec3 position,
										 glm::vec3 rotation,
										 float fovY,
										 float aspectRatio,
										 float nearPlane,
										 float farPlane) const noexcept override;
};