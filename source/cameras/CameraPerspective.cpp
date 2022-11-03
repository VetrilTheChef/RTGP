#include "CameraPerspective.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>


#include <iostream>
#include "glm/gtx/string_cast.hpp"

using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

CameraPerspective::CameraPerspective(vec3 newPosition,
									 vec3 newRotation,
									 float newfovY,
									 float newAspectRatio,
									 float newNearPlane,
									 float newFarPlane) noexcept :
	ICamera(),
	position(newPosition),
	rotation(newRotation),
	fovY(newfovY),
	aspectRatio(newAspectRatio),
	nearPlane(newNearPlane),
	farPlane(newFarPlane)
{
	// No negative near plane, no far plane closer than the near plane
	nearPlane = max(newNearPlane, 0.0f);
	farPlane = max(farPlane, nearPlane + 0.01f);

	// Compute the projection and view matrices
	updateProjectionMatrix();
	updateViewMatrix();

	// Compute the view vector
	updateViewVector();
}

CameraPerspective::~CameraPerspective() noexcept
{
}

glm::vec3 CameraPerspective::getPosition() const noexcept
{
	return position;
}

void CameraPerspective::setPosition(const glm::vec3 newPosition) noexcept
{
	position = newPosition;

	// Update the view matrix
	updateViewMatrix();

	// Update the view vector
	updateViewVector();
}

glm::vec3 CameraPerspective::getRotation() const noexcept
{
	return rotation;
}

void CameraPerspective::setRotation(const glm::vec3 newRotation) noexcept
{
	rotation = newRotation;

	// Update the view matrix
	updateViewMatrix();

	// Update the view vector
	updateViewVector();
}

float CameraPerspective::getVerticalFOV() const noexcept
{
	return fovY;
}

void CameraPerspective::setVerticalFOV(const float newFovY) noexcept
{
	fovY = max(newFovY, 0.0f);

	// Update the projection matrix
	updateProjectionMatrix();
}

float CameraPerspective::getAspectRatio() const noexcept
{
	return aspectRatio;
}

void CameraPerspective::setAspectRatio(const float newAspectRatio) noexcept
{
	aspectRatio = max(newAspectRatio, 0.0f);

	// Update the projection matrix
	updateProjectionMatrix();
}

float CameraPerspective::getNearPlane() const noexcept
{
	return nearPlane;
}

void CameraPerspective::setNearPlane(const float newNearPlane) noexcept
{
	nearPlane = max(newNearPlane, 0.0f);
	farPlane = max(farPlane, nearPlane + 0.01f);

	// Update the projection matrix
	updateProjectionMatrix();
}

float CameraPerspective::getFarPlane() const noexcept
{
	return farPlane;
}

void CameraPerspective::setFarPlane(const float newFarPlane) noexcept
{
	farPlane = newFarPlane > nearPlane ? newFarPlane : nearPlane + 0.01f;

	// Update the projection matrix
	updateProjectionMatrix();
}

void CameraPerspective::lookAt(const vec3 newTarget, const vec3 newUp) noexcept
{
	// Look at the target and update the Euler angles
	float rotX, rotY, rotZ = 0.0f;

	// Compute the transformation matrix
	mat4 lookAtMatrix = glm::lookAt(position, newTarget, newUp);

	// Extract the Euler angles and update them
	extractEulerAngleXYZ(lookAtMatrix, rotX, rotY, rotZ);

	rotation.x = degrees(rotX);
	rotation.y = degrees(rotY);
	rotation.z = degrees(rotZ);

	updateViewMatrix();
}

const glm::mat4 & CameraPerspective::getProjectionMatrix() const noexcept
{
	return projectionMatrix;
}

const glm::mat4 & CameraPerspective::getViewMatrix() const noexcept
{
	return viewMatrix;
}

const glm::vec3 & CameraPerspective::getViewVector() const noexcept
{
	return viewVector;
}

vec3 CameraPerspective::forward() const noexcept
{
	// Transform the forward vector by the inverse camera orientation
	// as we want to express a direction not in the camera space
	glm::vec4 forward = inverse(orientation()) * vec4(0.0f, 0.0f, -1.0f, 1.0f);

	return vec3(forward);
}

vec3 CameraPerspective::right() const noexcept
{
	// Transform the right vector by the inverse camera orientation
	// as we want to express a direction not in the camera space
	vec4 right = inverse(orientation()) * vec4(1.0f, 0.0f, 0.0f, 1.0f);

	return vec3(right);
}

vec3 CameraPerspective::up() const noexcept
{
	// Transform the up vector by the inverse camera orientation
	// as we want to express a direction not in the camera space
	vec4 up = inverse(orientation()) * vec4(0.0f, 1.0f, 0.0f, 1.0f);

	return vec3(up);
}

void CameraPerspective::update(double deltaSeconds) noexcept
{
}

void CameraPerspective::draw() noexcept
{
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED
///////////////////////////////////////////////////////////////////////////////

void CameraPerspective::updateProjectionMatrix()
{
	// Precompute a perspective projection matrix and save it
	projectionMatrix = perspective(radians(fovY),
								   aspectRatio,
								   nearPlane,
								   farPlane);
}

void CameraPerspective::updateViewMatrix()
{
	// Precompute a view matrix and save it
	viewMatrix = glm::lookAt(position, position + forward(), up());
}

void CameraPerspective::updateViewVector()
{
	// Precompute a view vector (the inverse of the forward vector's direction)
	// and save it
	viewVector = -1.0f * forward();
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////

mat4 CameraPerspective::orientation() const noexcept
{
	mat4 orientation = mat4(1.0f);
	orientation = rotate(orientation, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	orientation = rotate(orientation, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	orientation = rotate(orientation, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));

	return orientation;
}