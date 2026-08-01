#include "ImGuiManager.h"
#include <iostream>

ImGuiManager::ImGuiManager()
	: clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)), show_demo_window(true), show_another_window(false)
{
	// sensible defaults for the ImGui-side emitter config
	imguiEmitterConfig.position = glm::vec3(0.0f, 0.0f, 0.0f);
	imguiEmitterConfig.direction = glm::vec3(0.0f, 1.0f, -1.0f);
	imguiEmitterConfig.startColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	imguiEmitterConfig.endColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	imguiEmitterConfig.size = 1.0f;
	imguiEmitterConfig.speed = 1.0f;
	imguiEmitterConfig.speedVariation = 0.0f;
	imguiEmitterConfig.spread = 30.0f;
	imguiEmitterConfig.particleLifetime = 5.0f;
	imguiEmitterConfig.particleLifetimeVariation = 0.0f;
}

ImGuiManager::~ImGuiManager()
{
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
			if (selectedMode == 0 && continuousEmitter)
				activeEmitter = continuousEmitter;
			else if (selectedMode == 1 && burstEmitter)
				activeEmitter = burstEmitter;

			if (activeEmitter)
				imguiEmitterConfig = activeEmitter->getConfig();
		}

		if (selectedMode >= (int)ParticleGenMode::ShapeSphere)
		{
			ImGui::SliderInt("Shape Particle Count", &shapeParticleCount, 1, maxParticles);
			ImGui::Checkbox("Enable Motion", &shapeMotionEnabled);

			if (ImGui::Button("Regenerate Shape"))
				regenerateRequested = true;
		}
		ImGui::Separator();

		// Emitter controls
		if (ImGui::CollapsingHeader("Emitter Configuration", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Position
			float pos[3] = { imguiEmitterConfig.position.x, imguiEmitterConfig.position.y, imguiEmitterConfig.position.z };
			if (ImGui::DragFloat3("Position", pos, 0.1f)) {
				imguiEmitterConfig.position = glm::vec3(pos[0], pos[1], pos[2]);
			}

			// Direction
			float dir[3] = { imguiEmitterConfig.direction.x, imguiEmitterConfig.direction.y, imguiEmitterConfig.direction.z };
			if (ImGui::DragFloat3("Direction", dir, 0.1f)) {
				imguiEmitterConfig.direction = glm::normalize(glm::vec3(dir[0], dir[1], dir[2]));
			}

			// Colors
			float startColor[4] = { imguiEmitterConfig.startColor.r, imguiEmitterConfig.startColor.g, imguiEmitterConfig.startColor.b, imguiEmitterConfig.startColor.a };
			if (ImGui::ColorEdit4("Start Color", startColor)) {
				imguiEmitterConfig.startColor = glm::vec4(startColor[0], startColor[1], startColor[2], startColor[3]);
			}

			float endColor[4] = { imguiEmitterConfig.endColor.r, imguiEmitterConfig.endColor.g, imguiEmitterConfig.endColor.b, imguiEmitterConfig.endColor.a };
			if (ImGui::ColorEdit4("End Color", endColor)) {
				imguiEmitterConfig.endColor = glm::vec4(endColor[0], endColor[1], endColor[2], endColor[3]);
			}

			// Size & speed
			if (ImGui::DragFloat("Size", &imguiEmitterConfig.size, 0.01f, 0.0f, 100.0f)) {
			}
			if (ImGui::DragFloat("Speed", &imguiEmitterConfig.speed, 0.1f, 0.0f, 1000.0f)) {
			}
			if (ImGui::DragFloat("Speed Variation", &imguiEmitterConfig.speedVariation, 0.01f, 0.0f, 1000.0f)) {
			}

			// Spread
			if (ImGui::DragFloat("Spread (deg)", &imguiEmitterConfig.spread, 0.5f, 0.0f, 180.0f)) {
			}

			// Lifetime
			if (ImGui::DragFloat("Particle Lifetime", &imguiEmitterConfig.particleLifetime, 0.01f, 0.0f, 1000.0f)) {
			}
			if (ImGui::DragFloat("Lifetime Variation", &imguiEmitterConfig.particleLifetimeVariation, 0.01f, 0.0f, 1000.0f)) {
			}

			ImGui::Separator();

			if (activeEmitter == nullptr)
			{
				ImGui::TextColored(ImVec4(1, 0.5f, 0.2f, 1.0f), "No active emitter selected");
				ImGui::SameLine();
				if (ImGui::SmallButton("Apply (disabled)")) { /*noop*/ }
			}
			else
			{
				if (ImGui::Button("Apply to Active Emitter"))
				{
					ApplyConfigToActiveEmitter();
				}
				ImGui::SameLine();
				if (ImGui::SmallButton("Reset Emitter"))
				{
					// pull current emitter state back into the ImGui controls
					imguiEmitterConfig = activeEmitter->getConfig();
				}
			}
		}

		if (ImGui::CollapsingHeader("External Forces", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::DragFloat("Gravity", gravity, 0.01f, 0.0f, 100.0f)) {
			}
		}

		//ImGui::BeginGroup();
		//ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Settings");
		//ImGui::Separator();

		//ImGui::EndGroup();               // properly close the group
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

void ImGuiManager::SetContinuousEmitter(ContinuousEmitter* emitter) {
	continuousEmitter = emitter;
	if (!activeEmitter)
		activeEmitter = continuousEmitter;
}

void ImGuiManager::SetBurstEmitter(BurstEmitter* emitter) {
	burstEmitter = emitter;
}

const EmitterConfig& ImGuiManager::GetEmitterConfig() const
{
	return imguiEmitterConfig;
}

void ImGuiManager::SetEmitterConfig(const EmitterConfig& config)
{
	imguiEmitterConfig = config;
}

void ImGuiManager::ApplyConfigToActiveEmitter()
{
	if (!activeEmitter) return;

	activeEmitter->setPosition(imguiEmitterConfig.position);
	activeEmitter->setDirection(imguiEmitterConfig.direction);
	activeEmitter->setSpeed(imguiEmitterConfig.speed);
	activeEmitter->setSpeedVariation(imguiEmitterConfig.speedVariation);
	activeEmitter->setSpread(imguiEmitterConfig.spread);
	activeEmitter->setParticleLifetime(imguiEmitterConfig.particleLifetime);
	activeEmitter->setParticleLifetimeVariation(imguiEmitterConfig.particleLifetimeVariation);

	activeEmitter->setStartColor(imguiEmitterConfig.startColor);
	activeEmitter->setEndColor(imguiEmitterConfig.endColor);
	activeEmitter->setSize(imguiEmitterConfig.size);
}