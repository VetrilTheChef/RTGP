#pragma once

#include "interfaces/ISceneLoader.h"
#include <unordered_map>
#include <memory>
#include <string>
#include "rapidjson/Document.h"

// Forward declarations

class ICamera;
class ICameraFactory;
class ILight;
class ILightFactory;
class IModel;
class IModelFactory;

// This class represents a scene loaded from a json file using rapidjson.
// It is responsible for loading cameras, lights, and models.

class JsonSceneLoader : public ISceneLoader
{
	public:
		JsonSceneLoader(std::string newJsonPath,
						std::shared_ptr<ICameraFactory> newCameraFactory,
						std::shared_ptr<ILightFactory> newLightFactory,
						std::shared_ptr<IModelFactory> newModelFactory)
			noexcept;

		JsonSceneLoader(std::string newJsonPath,
						std::shared_ptr<ICameraFactory> && newCameraFactory,
						std::shared_ptr<ILightFactory> && newLightFactory,
						std::shared_ptr<IModelFactory> && newModelFactory)
			noexcept;

		~JsonSceneLoader() noexcept;

		// Load the scene
		virtual bool load() noexcept override;

		// Unload the scene
		virtual void unload() noexcept override;

		// Create the scene camera
		virtual bool createCamera(ISceneManager& manager) const noexcept override;

		// Creates the scene lights
		virtual bool createLights(ISceneManager & manager) const noexcept override;

		// Creates the scene lights
		virtual bool createModels(ISceneManager & manager) const noexcept override;

	protected:
		// The json file path
		std::string path = "";

		// The camera factory
		std::shared_ptr<ICameraFactory> cameraFactory;

		// The light factory
		std::shared_ptr<ILightFactory> lightFactory;

		// The model factory
		std::shared_ptr<IModelFactory> modelFactory;

		// The scene document structure
		rapidjson::Document scene;

	private:
		// Type definitions
		typedef bool (JsonSceneLoader::* createTypedCamera)(std::unique_ptr<ICamera> &,
															const rapidjson::Value &,
															ISceneManager & manager)
			const;

		typedef bool (JsonSceneLoader::* createTypedLight)(std::shared_ptr<ILight> &,
														   const rapidjson::Value &)
			const;

		typedef bool (JsonSceneLoader::* createTypedModel)(std::shared_ptr<IModel> &,
														   const rapidjson::Value &,
														   ISceneManager & manager)
			const;

		typedef std::unordered_map<std::string, createTypedCamera> CameraMap;

		typedef std::unordered_map<std::string, createTypedLight> LightMap;

		typedef std::unordered_map<std::string, createTypedModel> ModelMap;

		// A map coupling the camera types to the function pointers
		// used to create cameras of that type
		CameraMap cameraCreators;

		// A map coupling the light types to the function pointers
		// used to create lights of that type
		LightMap lightCreators;

		// A map coupling the model types to the function pointers
		// used to create models of that type
		ModelMap modelCreators;

		// Create a new perspective camera
		bool createPerspectiveCamera(std::unique_ptr<ICamera> & camera,
									 const rapidjson::Value & cameraData,
									 ISceneManager & manager)
			const noexcept;

		// Create a new point light
		bool createPointLight(std::shared_ptr<ILight> & light,
							  const rapidjson::Value & lightData)
			const noexcept;

		bool createStaticModel(std::shared_ptr<IModel> & model,
							   const rapidjson::Value & modelData,
							   ISceneManager & manager)
			const noexcept;
};