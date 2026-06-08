#include "ImGuiManager.h"
#include <iostream>

ImGuiManager::ImGuiManager()
    : clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)), show_demo_window(true), show_another_window(false)
{
}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::Init(GLFWwindow* window, const char* glsl_version)
{
    this->window = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::Render()
{
    // 1. Show demo window
    //if (show_demo_window)
        //ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a custom window
   
    static bool showSidebar = true;  // Sidebar visibility toggle

    ImGui::Begin("Sidebar Control", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f)); // Subtle blue button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.7f, 1.0f));
    if (ImGui::Button(showSidebar ? "Hide Controls" : "Show Controls", ImVec2(120, 30)))
    {
        showSidebar = !showSidebar;
    }
    ImGui::PopStyleColor(2);
    ImGui::End();

    if (showSidebar) {

        // Set position and size for left sidebar
        ImGui::SetNextWindowPos(ImVec2(0, 20)); // Adjust position slightly for main menu bar
        ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetIO().DisplaySize.y - 20)); // Adjust width as needed

        ImGui::Begin("Sidebar", &showSidebar, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));


        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Particle System Configuration"))
            {
                ImGui::MenuItem("Controls", nullptr, false, false);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 290.0f);

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Settings");
        ImGui::Separator();

        ImGui::EndGroup();               // properly close the group
        ImGui::PopTextWrapPos();
        ImGui::End();
    }
}

void ImGuiManager::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::SetupMenuBar(GLFWwindow* window, bool* should_close)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) { /* Handle new */ }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) { /* Handle open */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Handle save */ }
            if (ImGui::MenuItem("Save As...")) { /* Handle save as */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit Window", "Alt+F4")) { *should_close = true; }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

ImVec4& ImGuiManager::GetClearColor()
{
    return clear_color;
}

bool& ImGuiManager::ShowDemoWindow()
{
    return show_demo_window;
}

bool& ImGuiManager::ShowAnotherWindow()
{
    return show_another_window;
}