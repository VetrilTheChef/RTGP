#include "LightFactory.h"
#include "lights/PointLight.h"

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

LightFactory::LightFactory() noexcept
{
}

LightFactory::~LightFactory() noexcept
{
}

bool LightFactory::createPointLight(std::shared_ptr<ILight> & light,
									glm::vec3 position,
									glm::vec3 color,
									float intensity) const noexcept
{
	// Create a concrete implementation of a Point Light
	light = make_shared<PointLight>(position, color, intensity);

	// Return true if not null
	return light.get();
}
