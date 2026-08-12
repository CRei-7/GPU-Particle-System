#include "ImGuiManager.h"
#include <iostream>

ImGuiManager::ImGuiManager()
	: clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)), show_demo_window(true), show_another_window(false)
{
	// sensible defaults for the ImGui-side emitter config
	emitterConfig.position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	emitterConfig.direction = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	emitterConfig.startColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	emitterConfig.endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	emitterConfig.size = 1.0f;
	emitterConfig.speed = 1.0f;
	emitterConfig.speedVariation = 0.0f;
	emitterConfig.spread = 30.0f;
	emitterConfig.particleLifetime = 5.0f;
	emitterConfig.particleLifetimeVariation = 0.0f;
	emitterConfig.spawnRate = 600.0f;
	emitterConfig.emitterType = selectedMode;
	emitterConfig.deltaTime = 0.0f;
	emitterConfig.burstCount = 50;
	emitterConfig.padding = glm::vec2(0.0f);

	defaultConfig = emitterConfig;

	continuousConfig = emitterConfig;
	continuousConfig.emitterType = (int)ParticleGenMode::ContinuousEmitter;

	burstConfig = emitterConfig;
	burstConfig.spawnRate = 10.0f;
	burstConfig.emitterType = (int)ParticleGenMode::BurstEmitter;
	burstConfig.direction = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	shapeConfig = emitterConfig;
	shapeConfig.emitterType = (int)ParticleGenMode::ShapeSphere;

}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::StoreConfigForMode(int mode)
{
	if (mode == (int)ParticleGenMode::ContinuousEmitter)
		continuousConfig = emitterConfig;
	else if (mode == (int)ParticleGenMode::BurstEmitter)
		burstConfig = emitterConfig;
	else
		shapeConfig = emitterConfig;
}

void ImGuiManager::LoadConfigForMode(int mode)
{
	if (mode == (int)ParticleGenMode::ContinuousEmitter)
		emitterConfig = continuousConfig;
	else if (mode == (int)ParticleGenMode::BurstEmitter)
		emitterConfig = burstConfig;
	else
		emitterConfig = shapeConfig;

	emitterConfig.emitterType = mode;
}


void ImGuiManager::Init(GLFWwindow* window, const char* glsl_version, int MAX_PARTICLES)
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

	maxParticles = MAX_PARTICLES;
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

		ImGui::Text("Particle Count: %u", aliveCount ? *aliveCount : 0);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Particle System Configuration"))
			{
				ImGui::MenuItem("Controls", nullptr, false, false);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 290.0f);

		// Generation Mode selector
		const char* modeNames[] = { "Continuous Emitter", "Burst Emitter", "Sphere", "Disc", "Cube" };
		if (ImGui::Combo("Mode", &selectedMode, modeNames, IM_ARRAYSIZE(modeNames)))
		{
			StoreConfigForMode(lastSelectedMode);
			LoadConfigForMode(selectedMode);
			lastSelectedMode = selectedMode;
		}

		if (selectedMode >= (int)ParticleGenMode::ShapeSphere)
		{
			//imguiEmitterConfig.endColor.a = 1.0f;

			ImGui::SliderFloat("Shape Size", &emitterConfig.size, 0.1f, 10.0f);
			ImGui::SliderInt("Shape Particle Count", &shapeParticleCount, 1, maxParticles);
			ImGui::Checkbox("Create Hollow Shape", &shapeHollowEnabled);
			if (shapeHollowEnabled) {
				ImGui::SliderFloat("Hollow Size", &shapeHollowSize, 0.0f, emitterConfig.size - 0.01f);
			}
			else {
				shapeHollowSize = 0.0f; // Reset hollow size when hollow is disabled
			}

			ImGui::Checkbox("Enable Rotation", &shapeRotationEnabled);
			if(shapeRotationEnabled) {
				ImGui::SliderFloat("Rotation Speed", &angularVelocity, 0.0f, 10.0f);
			}
			else {
				angularVelocity = 0.0f; // Reset rotation speed when rotation is disabled
			}

			ImGui::Checkbox("Enable Random Motion", &shapeMotionEnabled);
			ImGui::Checkbox("Add Offsets", &shapeOffsetsEnabled);
			if (shapeOffsetsEnabled) {
				ImGui::SliderFloat("X Offset", &xOffset, 0.0f, 1.0f);
				ImGui::SliderFloat("Y Offset", &yOffset, 0.0f, 1.0f);
				ImGui::SliderFloat("Z Offset", &zOffset, 0.0f, 1.0f);
			}

			ImGui::Checkbox("Add Roughness", &shapeRoughnessEnabled);
			if (shapeRoughnessEnabled) {
				ImGui::SliderFloat("Maximum Offset", &maxOffset, 0.0f, 1.0f);
				ImGui::SliderFloat("Roughness Exponent", &roughnessExponent, 0.1f, 5.0f);
			}
			else {
				maxOffset = 0.0f; // Reset max offset when roughness is disabled
				roughnessExponent = 1.0f; // Reset roughness exponent when roughness is disabled
			}

			if (ImGui::RadioButton("Linear Gradient", selectedGradientMode == 0))
				selectedGradientMode = 0;

			ImGui::SameLine();

			if (ImGui::RadioButton("Radial Gradient", selectedGradientMode == 1))
				selectedGradientMode = 1;

			float gradSPos[3] = { gradientStartPos.x, gradientStartPos.y, gradientStartPos.z };
			if (ImGui::DragFloat3("Gradient Start Position", gradSPos, 0.1f)) {
				gradientStartPos = glm::vec3(gradSPos[0], gradSPos[1], gradSPos[2]);
			};

			float gradEPos[3] = { gradientEndPos.x, gradientEndPos.y, gradientEndPos.z };
			if (ImGui::DragFloat3("Gradient End Position", gradEPos, 0.1f)) {
				gradientEndPos = glm::vec3(gradEPos[0], gradEPos[1], gradEPos[2]);
			}

			if (ImGui::Button("Regenerate Shape"))
				regenerateRequested = true;
		}
		ImGui::Separator();

		// Emitter controls
		if (ImGui::CollapsingHeader("Emitter Configuration", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Position", glm::value_ptr(emitterConfig.position), 0.1f);

			ImGui::DragFloat3("Direction", glm::value_ptr(emitterConfig.direction), 0.1f);

			ImGui::ColorEdit4("Start Color", glm::value_ptr(emitterConfig.startColor));
			ImGui::ColorEdit4("End Color", glm::value_ptr(emitterConfig.endColor));

			ImGui::DragFloat("Speed", &emitterConfig.speed, 0.1f, 0.0f, 1000.0f);
			ImGui::DragFloat("Speed Variation", &emitterConfig.speedVariation, 0.01f, 0.0f, 1000.0f);

			ImGui::DragFloat("Spread (deg)", &emitterConfig.spread, 0.5f, 0.0f, 180.0f);

			ImGui::DragFloat("Particle Lifetime", &emitterConfig.particleLifetime, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("Lifetime Variation", &emitterConfig.particleLifetimeVariation, 0.01f, 0.0f, 1000.0f);

			ImGui::DragFloat("Spawn Rate", &emitterConfig.spawnRate, 0.1f, 0.0f, 1000.0f);
			ImGui::DragInt("Burst Count", &emitterConfig.burstCount, 1.0f, 1, 1000);

			ImGui::Separator();

			if (ImGui::Button("Reset to Defaults"))
			{
				emitterConfig = defaultConfig;
				emitterConfig.emitterType = selectedMode;
				if (selectedMode == 1)
					emitterConfig.spawnRate = 10.0f;
			}
		}

		if (ImGui::CollapsingHeader("External Forces", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (gravity)
				ImGui::DragFloat("Gravity", gravity, 0.01f, 0.0f, 100.0f);
		}

		if (ImGui::CollapsingHeader("Bloom Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (glowIntensity)  ImGui::DragFloat("Glow Intensity", glowIntensity, 0.01f, 0.0f, 10.0f);
			if (quadRadius)     ImGui::DragFloat("Quad Radius", quadRadius, 0.001f, 0.0f, 2.0f);
			if (bloomExposure)  ImGui::DragFloat("Bloom Exposure", bloomExposure, 0.01f, 0.0f, 10.0f);
			if (bloomStrength)  ImGui::DragFloat("Bloom Strength", bloomStrength, 0.01f, 0.0f, 10.0f);
		}

		if (ImGui::CollapsingHeader("Trail Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (trailWidth)  ImGui::DragFloat("Trail Width", trailWidth, 0.001f, 0.0f, 1.0f);
			if (trailAlpha)  ImGui::DragFloat("Trail Alpha", trailAlpha, 0.001f, 0.0f, 1.0f);
		}

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

const gpu::EmitterConfig& ImGuiManager::GetEmitterConfig() const {
	return emitterConfig;
}

void ImGuiManager::SetEmitterConfig(const gpu::EmitterConfig& config)
{
	emitterConfig = config;
}