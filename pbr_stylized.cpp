/*
PBR_Stylized

author: Marco Moretti

Personal Project for Real-Time Graphics Programming - a.a. 2020/2021
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
#include <glm/gtc/type_ptr.hpp>

// confirm that GLAD didn't include windows.h
#ifdef _WINDOWS_
#error windows.h was included!
#endif

// Project includes

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include "cameras/CameraPerspective.h"
#include "lights/PointLight.h"
#include "lights/includes/Lights.h"
#include "meshes/MeshAssImp.h"
#include "models/Model.h"
#include "shaders/loaders/FileShaderLoader.h"
#include "shaders/programs/ShaderProgram.h"
#include "shaders/programs/includes/MVPN.h"

// Project macros

#define SCENE "content/dev.scene"

// Function prototypes
void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);

void ToggleModelRotation();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

bool ModelRotation = true;

using namespace std;

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

    string meshPath = "";
    string vertPath = "";
    string fragPath = "";

    ifstream fileStream(SCENE, ios::in);

    if (fileStream.is_open())
    {
        string line;

        while (getline(fileStream, line))
        {
            regex reg("\\s+");

            sregex_token_iterator it(line.begin(), line.end(), reg, -1);
            sregex_token_iterator end;

            vector<string> tokens(it, end);

            if (tokens.size() == 2)
            {
                if (!_stricmp(tokens[0].c_str(), "meshPath"))
                {
                    meshPath = tokens[1];
                }
                else if (!_stricmp(tokens[0].c_str(), "vertShaderPath"))
                {
                    vertPath = tokens[1];
                }
                else if (!_stricmp(tokens[0].c_str(), "fragShaderPath"))
                {
                    fragPath = tokens[1];
                }
            }
            else
            {
                cout << "Scene: incorrect syntax at line \"" << line << "\"."
                    << endl;
            }
        }

        fileStream.close();
    }

    // Main scope
    {
        // MVPN matrices data structure
        MVPN mvpn;

        // Lights data structure
        Lights lights;

        // View vector
        glm::vec3 view;

        // Scene camera
        CameraPerspective camera(glm::vec3(0.0f, 1.0f, 3.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 90.0f,
                                 (float) WIDTH / (float) HEIGHT,
                                 0.1f,
                                 10.0f);

        // Scene lights
        vector<PointLight> pointLights;
        pointLights.emplace_back(glm::vec3(-3.0f, 2.0f, 0.0f),
                                 glm::vec3(0.0f, 0.5f, 0.5f),
                                 5.0f); // Left, Teal

        pointLights.emplace_back(glm::vec3(3.0f, 2.0f, 0.0f),
                                 glm::vec3(1.0f, 0.5f, 0.0f),
                                 5.0f); // Right, Orange

        pointLights.emplace_back(glm::vec3(0.0f, 1.0f, 3.0f),
                                 glm::vec3(1.0f, 1.0f, 1.0f),
                                 2.0f); // Front, White

        // Add the scene lights to the lights data structure
        for (int i = 0; i< pointLights.size(); i++)
        {
            PointLight & pointLight = pointLights.at(i);
            lights.Positions[i] = glm::vec4(pointLight.getPosition(), 1.0);
            lights.Colors[i] = glm::vec4(pointLight.getColor(), pointLight.getIntensity());
        }

        // Scene models
        Model model(make_unique<MeshAssImp>(meshPath),
                    make_shared<ShaderProgram>(make_shared<FileShaderLoader>(vertPath),
                                               make_shared<FileShaderLoader>(fragPath),
                                               mvpn,
                                               lights),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.33f, 0.33f, 0.33f));

        // Point the camera at the target
        camera.lookAt(model.getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));

        // Uncommenting this call will result in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glm::vec3 modelRotation = glm::vec3(0.0f);

        // Render loop
        while (!glfwWindowShouldClose(window))
        {
            // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
            glfwPollEvents();

            // Clear both the color and the depth buffer
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (ModelRotation)
            {
                // Make the model spin
                modelRotation = model.getRotation();
                modelRotation.y = (modelRotation.y + 1.0f) - (int)modelRotation.y / 360 * 360.0f;
                model.setRotation(modelRotation);
            }

            // Update the MVPN matrices
            mvpn.model = model.getModelMatrix();
            mvpn.view = camera.getViewMatrix();
            mvpn.projection = camera.getProjectionMatrix();
            mvpn.normal = transpose(inverse(mvpn.model));

            // Update the view vector
            view = camera.getViewVector();

            // Get the shader program of the current model
            shared_ptr<IShaderProgram> modelProgram = model.getProgram();

            // This should always be valid, but just in case
            if (modelProgram.get())
            {
                // Update the scene uniforms
                modelProgram->setViewVector(glm::value_ptr(view));
            }

            // Render the model
            model.render();

            // Swap the screen buffers
            glfwSwapBuffers(window);
        }

        // Main scope ends here and resources are released
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