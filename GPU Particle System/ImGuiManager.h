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

#include "ParticleGenMode.h"
#include "GPUStructures.h"

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

	void SetEmitterConfig(const gpu::EmitterConfig& config);
	const gpu::EmitterConfig& GetEmitterConfig() const;

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

	bool GetShapeHollowEnabled() { return shapeHollowEnabled; }

	float GetShapeHollowSize() { return shapeHollowSize; }

	bool GetShapeOffsetsEnabled() { return shapeOffsetsEnabled; }
	glm::vec3 GetShapeOffsets() { return glm::vec3(xOffset, yOffset, zOffset); }

	bool GetShapeRoughnessEnabled() { return shapeRoughnessEnabled; }
	float GetMaxOffset() { return maxOffset; }
	float GetRoughnessExponent() { return roughnessExponent; }

	bool GetShapeRotationEnabled() { return shapeRotationEnabled; }
	float GetAngularVelocity() { return angularVelocity; }

	float GetSpawnRate() { return emitterConfig.spawnRate; }
	float GetBurstCount() { return emitterConfig.burstCount; }

	void SetSpawnRate(float rate) { emitterConfig.spawnRate = rate; }
	void SetBurstCount(int count) { emitterConfig.burstCount = count; }

	void SetGlowIntensity(float* ptr) { glowIntensity = ptr; }
	void SetQuadRadius(float* ptr) { quadRadius = ptr; }
	void SetBloomExposure(float* ptr) { bloomExposure = ptr; }
	void SetBloomStrength(float* ptr) { bloomStrength = ptr; }

	glm::vec3 GetGradientStart() { return gradientStartPos; }
	glm::vec3 GetGradientEnd() { return gradientEndPos; }

	int GetSelectedGradientMode() { return selectedGradientMode; }

	void SetTrailWidth(float* ptr) { trailWidth = ptr; }
	void SetTrailAlpha(float* ptr) { trailAlpha = ptr; }

private:
    ImVec4 clear_color;
    bool show_demo_window = false;
    bool show_another_window = false;
    ImFont* menu_font = nullptr;

    GLFWwindow* window = nullptr; // Store the GLFW window pointer

	gpu::EmitterConfig emitterConfig{};

	gpu::EmitterConfig continuousConfig{};
	gpu::EmitterConfig burstConfig{};
	gpu::EmitterConfig shapeConfig{};
	gpu::EmitterConfig defaultConfig{};

	void StoreConfigForMode(int mode);
	void LoadConfigForMode(int mode);

	int selectedMode = 0; //0 = continuous, 1 = burst, 2 = sphere, 3 = disc, 4 = cube
	int lastSelectedMode = 0;

    float* gravity = nullptr;
	uint32_t* aliveCount = nullptr;
	bool shapeMotionEnabled = false;
	bool regenerateRequested = false;
	int shapeParticleCount = 1000; // Default particle count for shape generation

	int maxParticles = 0;

	bool shapeHollowEnabled = false;
	float shapeHollowSize = 0.0f; // Default hollow size for shape generation

	bool shapeOffsetsEnabled = false;
	float xOffset = 0.0f;
	float yOffset = 0.0f;
	float zOffset = 0.0f;

	bool shapeRoughnessEnabled = false; // Default roughness disabled
	float maxOffset = 0.0f; // Default maximum offset for roughness
	float roughnessExponent = 1.0f; // Default roughness exponent for roughness

	bool shapeRotationEnabled = false; // Default rotation disabled
	float angularVelocity = 0.0f; // Default rotation speed for shape rotation

	float* glowIntensity = nullptr;
	float* quadRadius = nullptr;
	float* bloomExposure = nullptr;
	float* bloomStrength = nullptr;

	glm::vec3 gradientStartPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 gradientEndPos = glm::vec3(1.0f, 1.0f, 1.0f);

	int selectedGradientMode = 0;

	float* trailWidth = nullptr;
	float* trailAlpha = nullptr;
};

#endif // IMGUIMANAGER_H