/*
PBR_Stylized

author: Marco Moretti

Personal Project for Real-Time Graphics Programming - a.a. 2022/2023
*/

/*
OpenGL coordinate system (right-handed)
positive X axis points right
positive Y axis points up
positive Z axis points "outside" the screen


                              Y
                              |
                              |
                              |________X
                             /
                            /
                           /
                          Z
*/

// THIS IS OPTIONAL AND NOT REQUIRED, ONLY USE THIS IF YOU DON'T WANT GLAD TO INCLUDE windows.h
// GLAD will include windows.h for APIENTRY if it was not previously defined.
// Make sure you have the correct definition for APIENTRY for platforms which define _WIN32 but don't use __stdcall

#ifdef _WIN32
#define APIENTRY __stdcall
#endif

#include <glad/glad.h>
#include <glfw/glfw3.h>

// confirm that GLAD didn't include windows.h
#ifdef _WINDOWS_
#error windows.h was included!
#endif

// Project includes

#include <iostream>
#include <memory>
#include <string>
#include "factories/CameraFactory.h"
#include "factories/LightFactory.h"
#include "factories/ModelFactory.h"
#include "gui/HUDImGui.h"
#include "scenes/loaders/JsonSceneLoader.h"
#include "scenes/managers/SceneManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "textures/FileTexture.h"

using namespace std;

// Project macros

// ...

// Function prototypes
void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);

void ToggleModelRotation();

// Scene JSON file
string sceneFile = "content/lambertian.scene";

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shadow Map dimensions
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// Render passes
//enum RenderPasses { SHADOWMAP, };

bool ModelRotation = true;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    std::cout << "Starting GLFW context" << std::endl;

    // Init GLFW
    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Stylized PBR", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    //glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    //glewInit();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize OpenGL context" << endl;

        return -1;
    }

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, & width, & height);
    glViewport(0, 0, width, height);

    // Enable the depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Main scope
    {
        // Create the factories
        shared_ptr<ICameraFactory> cameraFactory = make_shared<CameraFactory>();
        shared_ptr<ILightFactory> lightFactory = make_shared<LightFactory>();
        shared_ptr<IModelFactory> modelFactory = make_shared<ModelFactory>();

        // Create the scene loader
        shared_ptr<ISceneLoader> sceneLoader = make_shared<JsonSceneLoader>(
                                                sceneFile,
                                                cameraFactory,
                                                lightFactory,
                                                modelFactory);

        // Create the scene manager
        SceneManager sceneManager(sceneLoader, (float) WIDTH, (float) HEIGHT);

        // Create the HUD
        HUDImGui hud(window, sceneManager.lambertian);

        std::cout << "Loading scene..." << std::endl;

        sceneManager.load();

        // Uncommenting this call will result in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        std::cout << "Scene loaded." << std::endl;

        double time = glfwGetTime();

        std::cout << "Starting render loop." << std::endl;

        // Render loop
        while (!glfwWindowShouldClose(window))
        {
            // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
            glfwPollEvents();

            // Clear both the color and the depth buffer
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update the time passed since the last frame
            double deltaSeconds = time;
            time = glfwGetTime();
            deltaSeconds = max(time - deltaSeconds, 0.0);

            // Update the HUD
            hud.update(deltaSeconds);

            // Update the scene
            sceneManager.update(deltaSeconds);

            // Render the scene
            sceneManager.render();

            // Draw the HUD
            hud.draw();

            // Swap the screen buffers
            glfwSwapBuffers(window);
        }

        // Main scope ends here and resources are released

        std::cout << "Shutting down." << std::endl;
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        ToggleModelRotation();
}

void ToggleModelRotation()
{
    ModelRotation = !ModelRotation;
}