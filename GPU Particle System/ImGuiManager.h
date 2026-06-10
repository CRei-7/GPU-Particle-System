#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Emitter.h"
#include "EmitterConfig.h"
#include "ContinuousEmitter.h"

class ImGuiManager
{
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window, const char* glsl_version);
    void BeginFrame();
    void EndFrame();
    void Render();
    void Cleanup();

    void SetupMenuBar(GLFWwindow* window, bool* should_close);

    ImVec4& GetClearColor();
    bool& ShowDemoWindow();
    bool& ShowAnotherWindow();

    // Emitter integration
    void SetActiveEmitter(ContinuousEmitter* emitter);                      // Set which emitter ImGui should apply changes to
    const EmitterConfig& GetEmitterConfig() const;                // Get a copy/reference of the ImGui-ed config
    void SetEmitterConfig(const EmitterConfig& config);           // Replace ImGui's internal config

    void SetGravity(float* ptr) { gravity = ptr; }

private:
    ImVec4 clear_color;
    bool show_demo_window = false;
    bool show_another_window = false;
    ImFont* menu_font = nullptr;

    GLFWwindow* window = nullptr; // Store the GLFW window pointer

    // Local copy of emitter config exposed to ImGui controls
    EmitterConfig imguiEmitterConfig{};
    Emitter* activeEmitter = nullptr;

    void ApplyConfigToActiveEmitter();

    float* gravity = nullptr;
};

#endif // IMGUIMANAGER_H