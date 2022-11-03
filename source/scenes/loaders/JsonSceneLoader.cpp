#include "JsonSceneLoader.h"
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "cameras/interfaces/ICamera.h"
#include "factories/interfaces/ICameraFactory.h"
#include "factories/interfaces/ILightFactory.h"
#include "factories/interfaces/IModelFactory.h"
#include "lights/interfaces/ILight.h"
#include "models/interfaces/IModel.h"
#include "scenes/managers/interfaces/ISceneManager.h"

using namespace std;
using namespace glm;
using namespace rapidjson;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

JsonSceneLoader::JsonSceneLoader(std::string newJsonPath,
                                 std::shared_ptr<ICameraFactory> newCameraFactory,
                                 std::shared_ptr<ILightFactory> newLightFactory,
                                 std::shared_ptr<IModelFactory> newModelFactory)
    noexcept :
    ISceneLoader(),
    path(newJsonPath),
    cameraFactory(newCameraFactory),
    lightFactory(newLightFactory),
    modelFactory(newModelFactory)
{
    cameraCreators.insert(make_pair("perspective", & JsonSceneLoader::createPerspectiveCamera));

    lightCreators.insert(make_pair("point", & JsonSceneLoader::createPointLight));

    modelCreators.insert(make_pair("static", & JsonSceneLoader::createStaticModel));
}

JsonSceneLoader::JsonSceneLoader(std::string newJsonPath,
                                 std::shared_ptr<ICameraFactory> && newCameraFactory,
                                 std::shared_ptr<ILightFactory> && newLightFactory,
                                 std::shared_ptr<IModelFactory> && newModelFactory)
    noexcept :
    JsonSceneLoader(newJsonPath,
                    newCameraFactory,
                    newLightFactory,
                    newModelFactory)
{
}

JsonSceneLoader::~JsonSceneLoader() noexcept
{
}

bool JsonSceneLoader::load() noexcept
{
    string jsonContent = "";
    ifstream fileStream(path, ios::in);

    if (fileStream.is_open())
    {
        stringstream buffer;

        buffer << fileStream.rdbuf();
        jsonContent = buffer.str();

        fileStream.close();

        scene.Parse(jsonContent.c_str());

        return true;
    }

    // Log a warning
    cout << "Scene: could not open scene file \"" << path << "\"." << endl;

    return false;
}

void JsonSceneLoader::unload() noexcept
{
    scene.Clear();
}

bool JsonSceneLoader::createCamera(ISceneManager& manager) const noexcept
{
    // If the json scene contains a camera
    if (scene.HasMember("camera"))
    {
        // Retrieve the camera data
        const Value & cameraData = scene["camera"];

        // If the camera data has a camera type
        if (cameraData.HasMember("type"))
        {
            // Retrieve the camera type
            const string cameraType = string(cameraData["type"].GetString());

            // Find the type's relevant creational function
            auto it = cameraCreators.find(cameraType);

            // If the type's a known type
            if (it != cameraCreators.end())
            {
                unique_ptr<ICamera> camera;

                // Create the typed camera
                // Had to look up the correct syntax for this line
                if ((this->* it->second)(camera, cameraData, manager))
                {
                    // If successful, pass it to the scene manager
                    manager.setCamera(camera);
                }
            }
            else
            {
                // Log a warning
                cout << "Scene loader: found unsupported camera type: "
                    << cameraType << "." << endl;
            }
        }
        else
        {
            // Log a warning
            cout << "Scene loader: could not read camera type parameter."
                << endl;
        }

        return true;
    }

    return false;
}

bool JsonSceneLoader::createLights(ISceneManager & manager) const noexcept
{
    // If the json scene contains lights
    if (scene.HasMember("lights"))
    {
        // Retrieve the lights data
        const Value & lights = scene["lights"];

        // Sanity check
        if (lights.IsArray())
        {
            // Iterate over the lights
            for (SizeType i = 0; i < lights.Size(); i++)
            {
                // Retrieve the light data
                const Value & lightData = lights[i];

                // If the light data has a light type
                if (lightData.HasMember("type"))
                {
                    // Retrieve the light type
                    const string lightType = string(lightData["type"].GetString());

                    // Find the type's relevant creational function
                    auto it = lightCreators.find(lightType);

                    // If the type's a known type
                    if (it != lightCreators.end())
                    {
                        shared_ptr<ILight> light;

                        // Create the typed light
                        if ((this->* it->second)(light, lightData))
                        {
                            // If successful, pass it to the scene manager
                            manager.addLight(light);
                        }
                    }
                    else
                    {
                        // Log a warning
                        cout << "Scene loader: found unsupported light type: "
                            << lightType << "." << endl;
                    }
                }
                else
                {
                    // Log a warning
                    cout << "Scene loader: could not read light type parameter."
                        << endl;
                }
            }

            return true;
        }
    }

    return false;
}

bool JsonSceneLoader::createModels(ISceneManager & manager) const noexcept
{
    // If the json scene contains models
    if (scene.HasMember("models"))
    {
        // Retrieve the models data
        const Value & models = scene["models"];

        // Sanity check
        if (models.IsArray())
        {
            // Iterate over the models
            for (SizeType i = 0; i < models.Size(); i++)
            {
                // Retrieve the model data
                const Value & modelData = models[i];

                // If the model data has a model type
                if (modelData.HasMember("type"))
                {
                    // Retrieve the model type
                    const string modelType = string(modelData["type"].GetString());

                    // Find the type's relevant creational function
                    auto it = modelCreators.find(modelType);

                    // If the type's a known type
                    if (it != modelCreators.end())
                    {
                        shared_ptr<IModel> model;

                        // Create the typed model
                        if ((this->*it->second)(model, modelData, manager))
                        {
                            // If successful, pass it to the scene manager
                            manager.addModel(model);
                        }
                    }
                    else
                    {
                        // Log a warning
                        cout << "Scene loader: found unsupported model type: "
                            << modelType << "." << endl;
                    }
                }
                else
                {
                    // Log a warning
                    cout << "Scene loader: could not read model type."
                        << endl;
                }
            }

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////

bool JsonSceneLoader::createPerspectiveCamera(std::unique_ptr<ICamera> & camera,
                                              const rapidjson::Value & cameraData,
                                              ISceneManager & manager)
    const noexcept
{
    camera.reset();

    // Check that the camera data contains all the necessary parameters
    if (cameraData.HasMember("position") && cameraData.HasMember("rotation") &&
        cameraData.HasMember("fovY") && cameraData.HasMember("nearPlane") &&
        cameraData.HasMember("farPlane"))
    {
        // Retrieve the parameters' json values
        const Value & cameraPos = cameraData["position"];
        const Value & cameraRot = cameraData["rotation"];
        const Value & cameraFovY = cameraData["fovY"];
        const Value & cameraNearPlane = cameraData["nearPlane"];
        const Value & cameraFarPlane = cameraData["farPlane"];

        // Sanity checks on retrieved json data
        if ((cameraPos.Size() == 3) &&
            cameraPos[0].IsFloat() &&
            cameraPos[1].IsFloat() &&
            cameraPos[2].IsFloat() &&
            (cameraRot.Size() == 3) &&
            cameraRot[0].IsFloat() &&
            cameraRot[1].IsFloat() &&
            cameraRot[2].IsFloat() &&
            cameraFovY.IsFloat() &&
            cameraNearPlane.IsFloat() &&
            cameraFarPlane.IsFloat())
        {
            // Check the camera factory
            if (cameraFactory.get())
            {
                // Create the camera parameters from the json values
                vec3 position = vec3(cameraPos[0].GetFloat(),
                                     cameraPos[1].GetFloat(),
                                     cameraPos[2].GetFloat());
                vec3 rotation = vec3(cameraRot[0].GetFloat(),
                                     cameraRot[1].GetFloat(),
                                     cameraRot[2].GetFloat());
                float fovY = cameraFovY.GetFloat();
                float nearPlane = cameraNearPlane.GetFloat();
                float farPlane = cameraFarPlane.GetFloat();

                // Compute the aspect ratio
                float aspectRatio = manager.GetViewportWidth() / manager.GetViewportHeight();

                // Forward the creation request to the camera factory
                return cameraFactory->createPerspectiveCamera(camera, position,
                                                              rotation, fovY,
                                                              aspectRatio,
                                                              nearPlane,
                                                              farPlane);
            }
            else
            {
                // Log a warning
                cout << "Scene Loader: camera factory is null." << endl;

                return false;
            }
        }
    }

    // This branch is taken if there was a problem with the json data
    // Log a warning
    cout << "Scene Loader: could not read perspective camera parameters." << endl;

    return false;
}

bool JsonSceneLoader::createPointLight(std::shared_ptr<ILight> & light,
                                       const rapidjson::Value & lightData)
    const noexcept
{
    light.reset();

    // Check that the light data contains all the necessary parameters
    if (lightData.HasMember("position") && lightData.HasMember("color") &&
        lightData.HasMember("intensity"))
    {
        // Retrieve the parameters' json values
        const Value & lightPos = lightData["position"];
        const Value & lightCol = lightData["color"];
        const Value & lightInt = lightData["intensity"];

        // Sanity checks on retrieved json data
        if ((lightPos.Size() == 3) &&
            lightPos[0].IsFloat() &&
            lightPos[1].IsFloat() &&
            lightPos[2].IsFloat() &&
            (lightCol.Size() == 3) &&
            lightCol[0].IsFloat() &&
            lightCol[1].IsFloat() &&
            lightCol[2].IsFloat() &&
            lightInt.IsFloat())
        {
            // Check the light factory
            if (lightFactory.get())
            {
                // Create the light parameters from the json values
                vec3 position = vec3(lightPos[0].GetFloat(),
                                     lightPos[1].GetFloat(),
                                     lightPos[2].GetFloat());
                vec3 color = vec3(lightCol[0].GetFloat(),
                                  lightCol[1].GetFloat(),
                                  lightCol[2].GetFloat());
                float intensity = lightInt.GetFloat();

                // Forward the creation request to the light factory
                return lightFactory->createPointLight(light, position, color, intensity);
            }
            else
            {
                // Log a warning
                cout << "Scene Loader: light factory is null." << endl;

                return false;
            }
        }
    }

    // This branch is taken if there was a problem with the json data
    // Log a warning
    cout << "Scene Loader: could not read point light parameters." << endl;

    return false;
}

bool JsonSceneLoader::createStaticModel(std::shared_ptr<IModel> & model,
                                        const rapidjson::Value & modelData,
                                        ISceneManager & manager)
    const noexcept
{
    model.reset();

    // Check that the model data contains all the necessary parameters
    if (modelData.HasMember("mesh") &&
        modelData.HasMember("vertexShader") &&
        modelData.HasMember("fragmentShader") &&
        modelData.HasMember("textures") &&
        modelData["textures"].HasMember("albedo") &&
        modelData["textures"].HasMember("normals") &&
        modelData["textures"].HasMember("roughness") &&
        modelData.HasMember("position") &&
        modelData.HasMember("rotation") &&
        modelData.HasMember("scale"))
    {
        // Retrieve the parameters' json values
        const Value & modelMesh = modelData["mesh"];
        const Value & modelVS = modelData["vertexShader"];
        const Value & modelFS = modelData["fragmentShader"];
        const Value & modelAlbedo = modelData["textures"]["albedo"];
        const Value & modelNormals = modelData["textures"]["normals"];
        const Value & modelRoughness = modelData["textures"]["roughness"];
        const Value & modelPos = modelData["position"];
        const Value & modelRot = modelData["rotation"];
        const Value & modelSc = modelData["scale"];

        // Sanity checks on retrieved json data
        if (modelMesh.IsString() &&
            modelVS.IsString() &&
            modelFS.IsString() &&
            modelAlbedo.IsString() &&
            modelNormals.IsString() &&
            modelRoughness.IsString() &&
            (modelPos.Size() == 3) &&
            modelPos[0].IsFloat() &&
            modelPos[1].IsFloat() &&
            modelPos[2].IsFloat() &&
            (modelRot.Size() == 3) &&
            modelRot[0].IsFloat() &&
            modelRot[1].IsFloat() &&
            modelRot[2].IsFloat() &&
            (modelSc.Size() == 3) &&
            modelSc[0].IsFloat() &&
            modelSc[1].IsFloat() &&
            modelSc[2].IsFloat())
        {            
            // Check the model factory
            if (modelFactory.get())
            {
                // Create the model parameters from the json values
                string meshPath = modelMesh.GetString();
                string vsPath = modelVS.GetString();
                string fsPath = modelFS.GetString();
                string albedo = modelAlbedo.GetString();
                string normals = modelNormals.GetString();
                string roughness = modelRoughness.GetString();
                vec3 position = vec3(modelPos[0].GetFloat(),
                                     modelPos[1].GetFloat(),
                                     modelPos[2].GetFloat());
                vec3 rotation = vec3(modelRot[0].GetFloat(),
                                     modelRot[1].GetFloat(),
                                     modelRot[2].GetFloat());
                vec3 scale = vec3(modelSc[0].GetFloat(),
                                  modelSc[1].GetFloat(),
                                  modelSc[2].GetFloat());

                // Forward the creation request to the model factory
                return modelFactory->createStaticModel(model, meshPath,
                                                       vsPath, fsPath,
                                                       albedo, normals, roughness,
                                                       manager.mvpn,
                                                       manager.lights,
                                                       manager.lambertian,
                                                       position,
                                                       rotation,
                                                       scale);
            }
            else
            {
                // Log a warning
                cout << "Scene Loader: model factory is null." << endl;

                return false;
            }
        }
    }

    // This branch is taken if there was a problem with the json data
    // Log a warning
    cout << "Scene Loader: could not read static model parameters." << endl;

    return false;
}