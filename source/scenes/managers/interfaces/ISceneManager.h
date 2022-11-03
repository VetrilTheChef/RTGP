#pragma once

#include <memory>
#include "lights/includes/Lights.h"
#include "shaders/programs/includes/MVPN.h"
#include "shaders/programs/includes/Lambertian.h"

// Forward declarations

class ICamera;
class ILight;
class IModel;
class ISceneLoader;

// The interface that Scene Manager classes must implement

class ISceneManager
{
	public:
		// The scene's loader
		std::shared_ptr<ISceneLoader> sceneLoader;

		// The scene's MVPN struct (used to send data to shaders)
		MVPN mvpn;

		// The scene's Lights struct (used to send data to shaders)
		Lights lights;

		// The scene's Lambertian struct (used to send data to shaders)
		Lambertian lambertian;

		virtual ~ISceneManager() noexcept {};

		// Load the scene
		virtual bool load() noexcept = 0;

		// Unload the scene
		virtual void unload() noexcept = 0;

		// Get the scene's viewport width
		virtual const float GetViewportWidth() const noexcept = 0;

		// Get the scene's viewport height
		virtual const float GetViewportHeight() const noexcept = 0;

		// Set the scene's camera
		virtual void setCamera(std::unique_ptr<ICamera> & newCamera) noexcept = 0;

		// Add a light to the scene
		virtual void addLight(std::shared_ptr<ILight> & newLight) noexcept = 0;

		// Add a model to the scene
		virtual void addModel(std::shared_ptr<IModel> & newModel) noexcept = 0;

		// Update the scene
		virtual void update(double deltaSeconds) noexcept = 0;

		// Render the scene
		virtual void render() noexcept = 0;

	protected:
		ISceneManager() {};

		ISceneManager(std::shared_ptr<ISceneLoader> newSceneLoader) noexcept :
			sceneLoader(std::move(newSceneLoader))
		{};

		// Disallowed - no need for 2 instances of the same scene manager
		ISceneManager(const ISceneManager & copy) = delete;
		ISceneManager& operator= (const ISceneManager & copy) = delete;

		// Disallowed - no need to move a scene manager
		ISceneManager(ISceneManager && move) = delete;
		ISceneManager & operator= (ISceneManager && move) = delete;
};