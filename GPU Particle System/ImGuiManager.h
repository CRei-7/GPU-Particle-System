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
#include "BurstEmitter.h"
#include "ParticleGenMode.h"

class ImGuiManager
{
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window, const char* glsl_version, int MAX_PARTICLES);
    void BeginFrame();
    void EndFrame();
    void Render();
    void Cleanup();

    void SetupMenuBar(GLFWwindow* window, bool* should_close);

    ImVec4& GetClearColor();
    bool& ShowDemoWindow();
    bool& ShowAnotherWindow();

    // Emitter integration
    void SetContinuousEmitter(ContinuousEmitter* emitter);
    void SetBurstEmitter(BurstEmitter* emitter);

    const EmitterConfig& GetEmitterConfig() const;                // Get a copy/reference of the ImGui-ed config
    void SetEmitterConfig(const EmitterConfig& config);           // Replace ImGui's internal config

    int GetSelectedMode() const { return selectedMode; }

    void SetGravity(float* ptr) { gravity = ptr; }
	void SetCountData(uint32_t* ptr) { aliveCount = ptr; }

    bool GetShapeMotionEnabled() { return shapeMotionEnabled; }

    bool RegenerateRequested() {
        bool r = regenerateRequested;
		regenerateRequested = false; // Reset after returning true once
        return r;
    }

	int GetShapeParticleCount() { return shapeParticleCount; }

	int GetSelectedMode() { return selectedMode; }

private:
    ImVec4 clear_color;
    bool show_demo_window = false;
    bool show_another_window = false;
    ImFont* menu_font = nullptr;

    GLFWwindow* window = nullptr; // Store the GLFW window pointer

    // Local copy of emitter config exposed to ImGui controls
    EmitterConfig imguiEmitterConfig{};
    Emitter* activeEmitter = nullptr;

    ContinuousEmitter* continuousEmitter = nullptr;
    BurstEmitter* burstEmitter = nullptr;

	int selectedMode = 0; //0 = continuous, 1 = burst, 2 = sphere, 3 = disc, 4 = cube

    void ApplyConfigToActiveEmitter();

    float* gravity = nullptr;
	uint32_t* aliveCount = nullptr;
	bool shapeMotionEnabled = false;
	bool regenerateRequested = false;
	int shapeParticleCount = 1000; // Default particle count for shape generation

	int maxParticles;
};

#endif // IMGUIMANAGER_H