#pragma once

#include "interfaces/ISceneManager.h"
#include <memory>
#include <string>
#include <vector>

// Forward declarations

class ICamera;

// This class represents a 3D scene.
// It is responsible for organizing and rendering through OpenGL entities such
// as cameras, lights, and models.

class SceneManager : public ISceneManager
{
	public:
		SceneManager(std::shared_ptr<ISceneLoader> newSceneLoader,
					 float newViewportWidth,
					 float newViewportHeight) noexcept;

		~SceneManager() noexcept;

		// Load the scene
		virtual bool load() noexcept override;

		// Unload the scene
		virtual void unload() noexcept override;

		// Get the scene's viewport width
		virtual const float GetViewportWidth() const noexcept override;

		// Get the scene's viewport height
		virtual const float GetViewportHeight() const noexcept override;

		// Set the scene's camera
		virtual void setCamera(std::unique_ptr<ICamera> & newCamera) noexcept override;

		// Add a light to the scene
		virtual void addLight(std::shared_ptr<ILight> & newLight) noexcept override;

		// Add a model to the scene
		virtual void addModel(std::shared_ptr<IModel> & newModel) noexcept override;

		// Update the scene
		virtual void update(double deltaSeconds) noexcept override;

		// Render the scene
		virtual void render() noexcept override;

	protected:
		// The viewport width
		float viewportWidth = 0.0;

		// The viewport height
		float viewportHeight = 0.0;

		// The scene camera
		std::shared_ptr<ICamera> sceneCamera;

		// The scene lights
		std::vector<std::shared_ptr<ILight>> sceneLights;

		// The scene models
		std::vector<std::shared_ptr<IModel>> sceneModels;
};