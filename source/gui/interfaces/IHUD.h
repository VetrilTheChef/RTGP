#pragma once

// Forward declarations
struct GLFWwindow;
struct Lambertian;

// The interface that HUD classes must implement

class IHUD
{
	public:
		virtual ~IHUD() noexcept {};

		// Update the HUD
		virtual void update(double deltaSeconds) noexcept = 0;

		// Draw the HUD
		virtual void draw() const noexcept = 0;

	protected:
		// Disallowed - must provide a GLFW window and the shader data structs
		IHUD() = delete;

		IHUD(GLFWwindow * newWindow, Lambertian & newLambertian) noexcept {};

		// Disallowed - no need for 2 instances of the same HUD
		IHUD(const IHUD & copy)  = delete;
		IHUD & operator= (const IHUD & copy) = delete;

		// Disallowed - no need to move a HUD
		IHUD(IHUD && move) = delete;
		IHUD & operator= (IHUD && move) = delete;
};