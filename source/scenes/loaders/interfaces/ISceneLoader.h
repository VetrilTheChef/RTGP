#pragma once

// Forward declarations

class ISceneManager;

// The interface that Scene Loader classes must implement

class ISceneLoader
{
	public:
		virtual ~ISceneLoader() noexcept {};

		// Load the scene
		virtual bool load() noexcept = 0;

		// Unload the scene
		virtual void unload() noexcept = 0;

		// Create the scene camera
		virtual bool createCamera(ISceneManager & manager) const noexcept = 0;

		// Creates the scene lights
		virtual bool createLights(ISceneManager & manager) const noexcept = 0;

		// Creates the scene models
		virtual bool createModels(ISceneManager & manager) const noexcept = 0;

	protected:
		ISceneLoader() {};

		// Disallowed - no need for 2 instances of the same scene loader
		ISceneLoader(const ISceneLoader & copy) = delete;
		ISceneLoader& operator= (const ISceneLoader & copy) = delete;

		// Disallowed - no need to move a scene loader
		ISceneLoader(ISceneLoader && move) = delete;
		ISceneLoader& operator= (ISceneLoader && move) = delete;
};