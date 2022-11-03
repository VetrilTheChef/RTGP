#pragma once

#include "interfaces/IHUD.h"

// Forward declarations
// ...

// This class represents a HUD which makes use of ImGui.
// It is responsible for creating and rendering widgets.

class HUDImGui : public IHUD
{
	public:
		HUDImGui(GLFWwindow * newWindow, Lambertian & newLambertian) noexcept;

		~HUDImGui() noexcept;

		// Update the HUD
		virtual void update(double deltaSeconds) noexcept override;

		// Draw the HUD
		virtual void draw() const noexcept override;

	protected:
		// The GLFW window this HUD renders to
		GLFWwindow * window;

		// The pointer to the Lambertian PBR / NPR variable
		// The UI will update its value as needed
		float * PBRtoNPR = nullptr;

		// The pointer to the Lambertian half-lambert variable
		// The UI will update its value as needed
		bool * halfLambert = nullptr;

		// The address of the Lambertian half-lambert bias variable
		// The UI will update its value as needed
		float * halfLambertBias = nullptr;

		// The address of the Lambertian color shades variable
		// The UI will update its value as needed
		int * colorShades = nullptr;
		
		// The address of the Lambertian blended shade percent variable
		// The UI will update its value as needed
		float * blendedShadePercent = nullptr;

		// The address of the Lambertian hsv shift variable
		// The UI will update its value as needed
		bool * hsvShift = nullptr;
};