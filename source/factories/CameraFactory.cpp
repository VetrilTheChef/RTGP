#include "CameraFactory.h"
#include "cameras/CameraPerspective.h"

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

CameraFactory::CameraFactory() noexcept
{
}

CameraFactory::~CameraFactory() noexcept
{
}

bool CameraFactory::createPerspectiveCamera(std::unique_ptr<ICamera> & camera,
											glm::vec3 position,
											glm::vec3 rotation,
											float fovY,
											float aspectRatio,
											float nearPlane,
											float farPlane) const noexcept
{
	// Create a concrete implementation of a Perspective Camera
	camera = make_unique<CameraPerspective>(position, rotation, fovY, aspectRatio,
											nearPlane, farPlane);

	// Return true if not null
	return camera.get();
}
