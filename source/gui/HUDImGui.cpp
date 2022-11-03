#include "HUDImGui.h"
#include "gui/includes/imgui.h"
#include "gui/includes/imgui_impl_glfw.h"
#include "gui/includes/imgui_impl_opengl3.h"
#include "shaders/programs/includes/Lambertian.h"

///////////////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////////////

HUDImGui::HUDImGui(GLFWwindow * newWindow, Lambertian & newLambertian) noexcept :
    IHUD(newWindow, newLambertian)
{
    window = newWindow;

    PBRtoNPR = & newLambertian.PBRtoNPR;
    halfLambert = & newLambertian.halfLambert;
    halfLambertBias = & newLambertian.halfLambertBias;
    colorShades = & newLambertian.colorShades;
    blendedShadePercent = & newLambertian.blendedShadePercent;
    hsvShift = & newLambertian.hsvShift;

    // Create the ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO & IO = ImGui::GetIO();

    // ImGui platform and renderer dependant setup
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    // Set the ImGui GUI style
    ImGui::StyleColorsDark();
}

HUDImGui::~HUDImGui() noexcept
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void HUDImGui::update(double deltaSeconds) noexcept
{
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // If the current UI widget is visible
    if (ImGui::Begin("SHADER PARAMETERS", NULL,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_AlwaysVerticalScrollbar |
        ImGuiWindowFlags_NoSavedSettings))
    {
        // Specify the UI Elements
        ImGui::Text("LAMBERTIAN");
        ImGui::Separator();
        ImGui::SliderFloat("PBR / NPR Shading", PBRtoNPR,
            0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Checkbox("Half-Lambert", halfLambert);
        ImGui::SliderFloat("Half-Lambert Bias", halfLambertBias,
            0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderInt("Color Shades", colorShades,
            1, 8, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderFloat("Blended Shade %", blendedShadePercent,
            0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Checkbox("HSV Shift", hsvShift);
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::Text("REFLECTIONS");
        ImGui::Separator();
        ImGui::Text("Blah blah...");
        ImGui::Separator();
        ImGui::NewLine();
    }

    // Regardless, finalize the current UI widget
    ImGui::End();
}

void HUDImGui::draw() const noexcept
{
    // Render the UI elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
