#include "PointLight.h"
#include "lights/interfaces/ILight.h"

using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

PointLight::PointLight(vec3 newPosition,
					   vec3 newColor,
					   float newIntensity) noexcept :
	ILight(newPosition,
		   newColor,
		   newIntensity)
{
	setPosition(newPosition);
	setColor(newColor);
	setIntensity(newIntensity);
}

PointLight::PointLight(const PointLight & copy) noexcept :
	ILight(copy)
{
	// Copy the data
	setPosition(copy.getPosition());
	setColor(copy.getColor());
	setIntensity(copy.getIntensity());
}

ILight & PointLight::operator=(const ILight & copy) noexcept
{
	// Copy the data
	setPosition(copy.getPosition());
	setIntensity(copy.getIntensity());
	setColor(copy.getColor());

	return * this;
}

PointLight::PointLight(ILight && move) noexcept :
	ILight(std::move(move))
{
	// Move the data
	setPosition(move.getPosition());
	setColor(move.getColor());
	setIntensity(move.getIntensity());

	move.setPosition(vec3(0.0f));
	move.setColor(vec3(0.0f));
	move.setIntensity(0.0f);
}

ILight & PointLight::operator=(ILight && move) noexcept
{
	// Move the data
	setPosition(move.getPosition());
	setIntensity(move.getIntensity());
	setColor(move.getColor());

	move.setPosition(vec3(0.0f));
	move.setColor(vec3(0.0f));
	move.setIntensity(0.0f);

	return * this;
}

PointLight::~PointLight() noexcept
{
}

vec3 PointLight::getPosition() const noexcept
{
	return position;
}

void PointLight::setPosition(const vec3 newPosition) noexcept
{
	position = newPosition;
}

float PointLight::getIntensity() const noexcept
{
	return color.w;
}

void PointLight::setIntensity(const float newIntensity) noexcept
{
	// Can't have a negative intensity
	color.w = max(newIntensity, 0.0f);
}

vec3 PointLight::getColor() const noexcept
{
	return vec3(color);
}

void PointLight::setColor(const vec3 newColor) noexcept
{
	// TODO: check that newColor is a valid SRGB color
	color.x = newColor.x;
	color.y = newColor.y;
	color.z = newColor.z;
}

void PointLight::draw() noexcept
{
	// TODO: draw a simple placeholder on screen
}