#include "SceneManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "cameras/interfaces/ICamera.h"
#include "lights/interfaces/ILight.h"
#include "models/interfaces/IModel.h"
#include "scenes/loaders/interfaces/ISceneLoader.h"

using namespace std;
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

SceneManager::SceneManager(std::shared_ptr<ISceneLoader> newSceneLoader,
                           float newViewportWidth,
                           float newViewportHeight)
    noexcept :
    ISceneManager(newSceneLoader),
    viewportWidth(newViewportWidth),
    viewportHeight(newViewportHeight)
{
    sceneCamera.reset();
    sceneLights.clear();
    sceneModels.clear();
}

SceneManager::~SceneManager() noexcept
{
    unload();
}

bool SceneManager::load() noexcept
{
    // The result of loading
    bool result = true;

    // Try to load the scene 
    if (sceneLoader.get() && sceneLoader->load())
    {
        // Create the camera
        result &= sceneLoader->createCamera(* this);
        
        // Create the lights
        result &= sceneLoader->createLights(* this);

        // Create the models
        result &= sceneLoader->createModels(* this);
    }
    else
    {
        // Log the error
        cout << "Scene manager: could not load scene." << endl;

        result = false;
    }

    return result;
}

const float SceneManager::GetViewportWidth() const noexcept
{
    return viewportWidth;
}

const float SceneManager::GetViewportHeight() const noexcept
{
    return viewportHeight;
}

void SceneManager::setCamera(std::unique_ptr<ICamera> & newCamera) noexcept
{
    sceneCamera = move(newCamera);
}

void SceneManager::unload() noexcept
{
    sceneModels.clear();
    sceneLights.clear();
    sceneCamera.reset();
}

void SceneManager::addLight(std::shared_ptr<ILight> & newLight) noexcept
{
    sceneLights.push_back(newLight);

    // Rebuild the lights data structure
    for (int i = 0; i < sceneLights.size(); i++)
    {
        lights.Positions[i] = glm::vec4(sceneLights[i]->getPosition(), 1.0);
        lights.Colors[i] = glm::vec4(sceneLights[i]->getColor(),
                                     sceneLights[i]->getIntensity());
    }
}

void SceneManager::addModel(std::shared_ptr<IModel> & newModel) noexcept
{
    sceneModels.push_back(newModel);
}

void SceneManager::update(double deltaSeconds) noexcept
{
    // If the camera is not null
    if (sceneCamera.get())
    {
        // Update the camera
        sceneCamera->update(deltaSeconds);
    }

    // Iterate through the lights
    for (shared_ptr<ILight> & light : sceneLights)
    {
        // If the light is not null
        if (light.get())
        {
            // Update the light
            light->update(deltaSeconds);
        }
    }

    // Iterate through the models
    for (shared_ptr<IModel> & model : sceneModels)
    {
        // If the model is not null
        if (model.get())
        {
            // Update the model
            model->update(deltaSeconds);
        }
    }
}

void SceneManager::render() noexcept
{
    // If there is a camera to render through
    if (sceneCamera.get())
    {
        // Update the per-frame MVPN struct elements
        mvpn.view = sceneCamera->getViewMatrix();
        mvpn.projection = sceneCamera->getProjectionMatrix();

        // Update of the Lambertian struct elements is handled by ImGui

        // Iterate through the models
        for (shared_ptr<IModel> & model : sceneModels)
        {
            if (model.get() && model->program.get())
            {
                // Update the per-model MVPN struct elements
                mvpn.model = model->getModelMatrix();
                mvpn.normal = transpose(inverse(mvpn.model));

                // Update the model's scene uniforms
                model->program->setViewVector(glm::value_ptr(sceneCamera->getViewVector()));

                // Render the model
                model->render();
            }
            else
            {
                // Log the error
                cout << "Scene manager: could not render model." << endl;
            }
        }
    }
    else
    {
        // Log the error
        cout << "Scene manager: could not access camera." << endl;
    }
}
