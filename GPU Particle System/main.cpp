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
#include "ParticlePool.h"
#include "Emitter.h"
#include "ImGuiManager.h"
#include "ContinuousEmitter.h"
#include "EmitterConfig.h"
#include "BurstEmitter.h"

Window mainWindow;
Camera camera;
ParticlePool particlePool;

EmitterConfig config{
    glm::vec3(0.0f, 0.0f, 0.0f),      // position
    glm::vec3(0.0f, 1.0f, -1.0f),     // direction
    glm::vec4(255.0f, 255.0f, 255.0f, 255.0f),// startColor
    glm::vec4(255.0f, 255.0f, 255.0f, 0.0f),// endColor
    1.0f,                             // size
    1.0f,                             // speed
    0.0f,                             // speedVariation
    30.0f,                            // Spread
    2.0f,                             // particleLifetime
    0.0f                              // particleLifetimeVariation
};

ContinuousEmitter continuousEmitter(config, 600.0f);//(config, spawn rate)
BurstEmitter burstEmitter(config, 100);

const int MAX_PARTICLES = 3000;

const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

// Vertex Shader
static const char* vShader = "./shader.vert";
static const char* fShader = "./shader.frag";

std::vector<Shader> shaderList;

GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

ImGuiManager imgui_manager;
const char* glsl_version = "#version 330";

void CreateShaders() {
	Shader* shaderProgram = new Shader();
	shaderProgram->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shaderProgram);
}

int main(){
	mainWindow = Window(SCR_WIDTH, SCR_HEIGHT);
	mainWindow.initialize();

	imgui_manager.Init(mainWindow.getGLFWwindow(), glsl_version);

    imgui_manager.SetActiveEmitter(&continuousEmitter);

    CreateShaders();

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

	particlePool = ParticlePool(MAX_PARTICLES);

	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), particlePool.particles.data(), GL_DYNAMIC_DRAW);// Syntax: (target, size, data, usage)

	glEnableVertexAttribArray(1);// This is for the instance offset attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));//Syntax: (index, size, type, normalized, stride, pointer)
	glVertexAttribDivisor(1, 1); // This tells OpenGL to update the offset attribute once per instance

	glEnableVertexAttribArray(2);// This is for the instance color attribute, 2 is the location in the shader
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
	glVertexAttribDivisor(2, 1); // This tells OpenGL to update the color attribute once per instance

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f); //(startPosition, startUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed)

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    //glm::radians converts degrees to radians
    //(fov, aspect, near field, far field)

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lasttime = 0.0f; // Time of last frame
	float gravity = 0.25f; // Gravity strength

    imgui_manager.SetGravity(&gravity);

    while (!mainWindow.getShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lasttime;
        lasttime = now;

        //continuousEmitter.update(deltaTime, particlePool);
        burstEmitter.update(deltaTime, particlePool);

        bool should_close = false;

        /*for (int i = 0; i < particlePool.capacity(); ++i) {
            Particle& p = particlePool.particles[i];

            if (p.life > 0.0f) {
                p.position += p.velocity * deltaTime;//Update particle position based on its speed

                p.velocity.y -= gravity * deltaTime; // Apply gravity to the particle's vertical speed

                p.life -= deltaTime;// Decrease particle life

                // If the particle is still alive, update its data in the instance VBO
                if (p.life > 0.0f) {// Update the particle's position or other properties here if needed
                    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Particle), sizeof(Particle), &particlePool.particles[i]);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                }
            }
        }*/

        for (int i = 0; i < particlePool.capacity(); ++i) {
            Particle& p = particlePool.particles[i];

            if (p.life > 0.0f) {
                p.position += glm::vec4(glm::vec3(p.velocity) * deltaTime, 0.0f);
                p.velocity.y -= gravity * deltaTime;
                p.life -= deltaTime;

                if (p.life <= 0.0f && p.velocity.w == 1.0f) {
                    p.velocity.w = 0.0f; // Clear flag before burst to be safe
                    burstEmitter.spawnBurst(particlePool, glm::vec3(p.position));
                }
                /*
                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Particle), sizeof(Particle), &p);
                glBindBuffer(GL_ARRAY_BUFFER, 0);*/
            }
        }

        particlePool.releaseDeadParticles();

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle),
            particlePool.particles.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getxChange(), mainWindow.getyChange());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imgui_manager.BeginFrame();

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
	glDeleteBuffers(1, &instanceVBO);
	shaderList[0].ClearShader();

    glfwTerminate();
    return 0;
}