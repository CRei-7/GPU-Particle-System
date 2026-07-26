#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
//#include "ParticlePool.h"
//#include "Emitter.h"
#include "ImGuiManager.h"
#include "ContinuousEmitter.h"
//#include "EmitterConfig.h"
#include "BurstEmitter.h"
#include "GPUStructures.h"

//For Nvidia GPU
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

Window mainWindow;
Camera camera;
ParticlePool particlePool;

gpu::EmitterConfig gpuConfig{
	glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),      // position
	glm::vec4(0.0f, 1.0f, -1.0f, 0.0f),     // direction
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),      // startColor
	glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),      // endColor
	1.0f,                             // size
	1.0f,                             // speed
	0.0f,                             // speedVariation
	30.0f,                            // spread
	2.0f,                             // particleLifetime
	0.0f,                             // particleLifetimeVariation
	600.0f,                           // spawnRate
	0,                                // emitterType
	0.01f,                            // deltaTime
	100,                              // burstCount
	glm::vec2(0.0f)                   // padding
};

//ContinuousEmitter continuousEmitter(config, 600.0f);//(config, spawn rate)
//BurstEmitter burstEmitter(config, 100);

gpu::EmitterConfig ToGpuEmitterConfig(const EmitterConfig& src, const gpu::EmitterConfig& base)
{
	gpu::EmitterConfig out = base;
	out.position = glm::vec4(src.position, 0.0f);
	out.direction = glm::vec4(src.direction, 0.0f);
	out.startColor = src.startColor;
	out.endColor = src.endColor;
	out.size = src.size;
	out.speed = src.speed;
	out.speedVariation = src.speedVariation;
	out.spread = src.spread;
	out.particleLifetime = src.particleLifetime;
	out.particleLifetimeVariation = src.particleLifetimeVariation;
	return out;
}

const int MAX_PARTICLES = 3000;

const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

// Vertex Shader
static const char* vShader = "./shader.vert";
static const char* fShader = "./shader.frag";

static const char* particleShader = "./particle.comp";
static const char* emitterShader = "./emitter.comp";
static const char* burstShader = "./burst.comp";

std::vector<Shader> shaderList;
Shader emitterProgram, particleProgram, burstProgram;
GLuint emitterComputeShader = 0, particleComputeShader = 0, burstComputeShader = 0;

GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

ImGuiManager imgui_manager;
const char* glsl_version = "#version 430";

//GPU Buffers
GLuint particleSSBO = 0;
GLuint particleCountSSBO = 0;      // Contains particleCount and freeListHead
GLuint freeListSSBO = 0;
GLuint accumumulatorSSBO = 0;
GLuint emitterConfigUBO = 0;
GLuint burstSSBO = 0;

void CreateShaders() {
	Shader* shaderProgram = new Shader();
	shaderProgram->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shaderProgram);
}

void CreateComputeShaders() {
	emitterProgram.CreateComputeShader(emitterShader);
	emitterComputeShader = emitterProgram.GetShaderID();

	particleProgram.CreateComputeShader(particleShader);
	particleComputeShader = particleProgram.GetShaderID();

	burstProgram.CreateComputeShader(burstShader);
	burstComputeShader = burstProgram.GetShaderID();
}

void InitializeGPUBuffers() {
	//Particle Buffer
	std::vector<gpu::Particle> initialParticles(MAX_PARTICLES);
	for (int i = 0; i < MAX_PARTICLES; ++i) {
		initialParticles[i] = gpu::Particle{
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			-1.0f,  // life = -1 means dead/unused
			-1.0f,  // maxLife = -1 means dead/unused
			glm::vec2(0.0f)
		};
	}

	glGenBuffers(1, &particleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(gpu::Particle), initialParticles.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Particle Count Buffer
	gpu::ParticleCountData countData = { 0, MAX_PARTICLES }; // Initially all particles are free
	glGenBuffers(1, &particleCountSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleCountSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(gpu::ParticleCountData), &countData, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleCountSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Free List Buffer
	std::vector<int> freeList(MAX_PARTICLES);
	for (int i = 0; i < MAX_PARTICLES; ++i) {
		freeList[i] = MAX_PARTICLES - 1 - i; // Fill in reverse order
	}
	glGenBuffers(1, &freeListSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, freeListSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(int), freeList.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, freeListSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Accumulator Buffer
	float accumulator = 0.0f;
	glGenBuffers(1, &accumumulatorSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumumulatorSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &accumulator, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, accumumulatorSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Burst Buffer
	glGenBuffers(1, &burstSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, burstSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + 10000 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW); //16 bytes for count + 10000 particles * 16 bytes each (vec4)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, burstSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Emitter Config UBO
	glGenBuffers(1, &emitterConfigUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, emitterConfigUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(gpu::EmitterConfig), &gpuConfig, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, emitterConfigUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int main() {
	mainWindow = Window(SCR_WIDTH, SCR_HEIGHT);
	mainWindow.initialize();

	//std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';

	imgui_manager.Init(mainWindow.getGLFWwindow(), glsl_version);

	ContinuousEmitter continuousEmitter(
		EmitterConfig{
			glm::vec3(0.0f, 0.0f, 0.0f),            // position
			glm::vec3(0.0f, 1.0f, -1.0f),           // direction
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),      // startColor
			glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),      // endColor
			1.0f,                             // size
			1.0f,                             // speed
			0.0f,                             // speedVariation
			30.0f,                            // spread
			2.0f,                             // particleLifetime
			0.0f                              // particleLifetimeVariation
		},
		600.0f                                // spawnRate
	);

	BurstEmitter burstEmitter(
		EmitterConfig{
			glm::vec3(0.0f, 0.0f, 0.0f),            // position
			glm::vec3(0.0f, 1.0f, 0.0f),            // direction
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),      // startColor
			glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),      // endColor
			1.0f,                             // size
			1.0f,                             // speed
			0.0f,                             // speedVariation
			30.0f,                            // spread
			2.0f,                             // particleLifetime
			0.0f                              // particleLifetimeVariation
		},
		100                                   // burst count
	);

	//imgui_manager.SetActiveEmitter(&continuousEmitter);
	imgui_manager.SetContinuousEmitter(&continuousEmitter);
	imgui_manager.SetBurstEmitter(&burstEmitter);

	CreateShaders();
	CreateComputeShaders();
	InitializeGPUBuffers();

	float vertices[] = {
		 0.005f,  0.005f, 0.0f,  // top right
		 0.005f, -0.005f, 0.0f,  // bottom right
		-0.005f, -0.005f, 0.0f,  // bottom left
		-0.005f,  0.005f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Vertex attribs for instanced rendering
	glBindBuffer(GL_ARRAY_BUFFER, particleSSBO);

	// Offsets/stride come from the GPU layout.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(gpu::Particle), (void*)offsetof(gpu::Particle, position));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(gpu::Particle), (void*)offsetof(gpu::Particle, color));
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(gpu::Particle), (void*)offsetof(gpu::Particle, life));
	glVertexAttribDivisor(3, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f); //(startPosition, startUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed)

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::radians converts degrees to radians
	//(fov, aspect, near field, far field)

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lasttime = (float)glfwGetTime(); // Time of last frame
	float gravity = 0.25f; // Gravity strength

	imgui_manager.SetGravity(&gravity);

	//std::cout << particlePool.particles[0].color.a << std::endl;

	while (!mainWindow.getShouldClose()) {
		static uint32_t frameCount = 0;
		++frameCount;

		GLfloat now = glfwGetTime();
		deltaTime = now - lasttime;
		lasttime = now;

		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getxChange(), mainWindow.getyChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gpu::EmitterConfig currentConfig = ToGpuEmitterConfig(imgui_manager.GetEmitterConfig(), gpuConfig);
		currentConfig.deltaTime = deltaTime;
		currentConfig.emitterType = imgui_manager.GetSelectedEmitter();

		glBindBuffer(GL_UNIFORM_BUFFER, emitterConfigUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(gpu::EmitterConfig), &currentConfig);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//Dispatches the emitter compute shader
		glUseProgram(emitterComputeShader);
		glUniform1ui(glGetUniformLocation(emitterComputeShader, "maxParticles"), MAX_PARTICLES);
		glUniform1ui(glGetUniformLocation(emitterComputeShader, "frameSeed"), frameCount);
		glDispatchCompute(1, 1, 1); // Only one thread group for serial spawning
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		//Dispatches burst compute shader if burst emitter is selected
		uint32_t zero = 0;// First reset burst counter
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, burstSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &zero);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		//Dispatches particle compute shader
		glUseProgram(particleComputeShader);
		glUniform1f(glGetUniformLocation(particleComputeShader, "gravity"), gravity);
		glUniform1ui(glGetUniformLocation(particleComputeShader, "maxParticles"), MAX_PARTICLES);
		glDispatchCompute((MAX_PARTICLES + 63) / 64, 1, 1); // 64 threads per workgroup
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

		glUseProgram(burstComputeShader);
		glUniform1ui(glGetUniformLocation(burstComputeShader, "maxParticles"), MAX_PARTICLES);
		glUniform1ui(glGetUniformLocation(burstComputeShader, "frameSeed"), frameCount);
		glDispatchCompute((MAX_PARTICLES + 63) / 64, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

		imgui_manager.BeginFrame();

		bool should_close = false;

		imgui_manager.SetupMenuBar(mainWindow.getGLFWwindow(), &should_close);
		if (should_close)
			glfwSetWindowShouldClose(mainWindow.getGLFWwindow(), true);
		imgui_manager.Render();
		imgui_manager.EndFrame();

		//imgui_manager.SetActiveEmitter(&emitter);

		shaderList[0].UseShader();

		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();

		//glUniformMatrix4fv binds a uniform to the shader, in this case we are binding the projection and view matrices to the shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));//This is for sending the projection matrix to the shader, 1 is for count, GL_FALSE is for whether we want to transpose the matrix or not, value_ptr is for converting the matrix to a pointer
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//glm::mat4 model = glm::mat4(1.0f);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 viewMat = camera.calculateViewMatrix();
		//view matrix stores data as
		//| right.x  up.x  front.x 0 |
		//| right.y  up.y  front.y 0 |
		//| right.z  up.z  front.z 0 |
		//| 0        0     0       1 |

		glm::mat3 invView = glm::mat3(glm::transpose(viewMat)); // Inverse of the view matrix for billboarding
		glm::vec3 cameraRight = invView[0]; // Right vector from the inverse view matrix, this gives us the first row of the inverse view matrix which corresponds to the right vector in world space
		glm::vec3 cameraUp = invView[1]; // Up vector from the inverse view matrix

		glUniform3fv(shaderList[0].GetCameraRightLocation(), 1, glm::value_ptr(cameraRight));
		glUniform3fv(shaderList[0].GetCameraUpLocation(), 1, glm::value_ptr(cameraUp));

		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_PARTICLES);

		mainWindow.swapBuffers();
	}

	imgui_manager.Cleanup();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &particleSSBO);
	glDeleteBuffers(1, &particleCountSSBO);
	glDeleteBuffers(1, &freeListSSBO);
	glDeleteBuffers(1, &accumumulatorSSBO);
	glDeleteBuffers(1, &emitterConfigUBO);
	glDeleteBuffers(1, &burstSSBO);

	emitterProgram.ClearShader();
	particleProgram.ClearShader();
	burstProgram.ClearShader();

	shaderList[0].ClearShader();

	glfwTerminate();
	return 0;
}