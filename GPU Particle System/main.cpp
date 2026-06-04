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

Window mainWindow;
Camera camera;

const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

// Vertex Shader
static const char* vShader = "./shader.vert";
static const char* fShader = "./shader.frag";

std::vector<Shader> shaderList;

GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

const unsigned int MAX_PARTICLES = 500;

struct Particle{
    glm::vec3 pos;
    glm::vec3 speed;
    unsigned char r, g, b, a; // Color
    //float size;
    //float angle;
    //float weight;
    float life; // Remaining life of the particle. if <0 : dead and unused.
};

Particle particlesContainer[MAX_PARTICLES];

void CreateShaders() {
	Shader* shaderProgram = new Shader();
	shaderProgram->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shaderProgram);
}

void setupParticle(int i) {
    float x = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
    float y = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
    float z = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
    particlesContainer[i].pos = glm::vec3(x, y, z);

	particlesContainer[i].speed = glm::vec3(0.0f, -0.5f, 0.0f); // Initial speed upwards

    /*float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;*/
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    particlesContainer[i].r = static_cast<unsigned char>(r * 255);
    particlesContainer[i].g = static_cast<unsigned char>(g * 255);
    particlesContainer[i].b = static_cast<unsigned char>(b * 255);
    particlesContainer[i].a = static_cast<unsigned char>(a * 255);

    particlesContainer[i].life = static_cast<float>(rand()) / RAND_MAX * 5.0f; // Random life between 0 and 5 seconds
}

int main(){
	mainWindow = Window(SCR_WIDTH, SCR_HEIGHT);
	mainWindow.initialize();

    CreateShaders();

    float vertices[] = {
         0.01f,  0.01f, 0.0f,  // top right
         0.01f, -0.01f, 0.0f,  // bottom right
        -0.01f, -0.01f, 0.0f,  // bottom left
        -0.01f,  0.01f, 0.0f   // top left 
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

    for (unsigned int i = 0; i < MAX_PARTICLES; ++i) {
		setupParticle(i);
    }

	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), particlesContainer, GL_STATIC_DRAW);// Syntax: (target, size, data, usage)

	glEnableVertexAttribArray(1);// This is for the instance offset attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle,pos));//Syntax: (index, size, type, normalized, stride, pointer)
	glVertexAttribDivisor(1, 1); // This tells OpenGL to update the offset attribute once per instance

	glEnableVertexAttribArray(2);// This is for the instance color attribute, 2 is the location in the shader
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Particle), (void*)offsetof(Particle, r));//GL_TRUE is for normalized, since we want to convert the unsigned byte to a float in the shader
	glVertexAttribDivisor(2, 1); // This tells OpenGL to update the color attribute once per instance

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f); //(startPosition, startUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed)

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    //glm::radians converts degrees to radians
    //(fov, aspect, near field, far field)

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lasttime = 0.0f; // Time of last frame

    while (!mainWindow.getShouldClose()){
		GLfloat now = glfwGetTime();
        deltaTime = now - lasttime;
		lasttime = now;

        for(int i = 0; i < MAX_PARTICLES; ++i) {
            if (particlesContainer[i].life > 0.0f) {
				//Update particle position based on its speed
				particlesContainer[i].pos += particlesContainer[i].speed * deltaTime;
                // Decrease particle life
                particlesContainer[i].life -= deltaTime;
                // If the particle is still alive, update its data in the instance VBO
				if (particlesContainer[i].life > 0.0f) {// Update the particle's position or other properties here if needed
                    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Particle), sizeof(Particle), &particlesContainer[i]);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                }
            }
            else {
                // Respawn the particle
				setupParticle(i);
            }
		}

        glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getxChange(), mainWindow.getyChange());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &instanceVBO);
	shaderList[0].ClearShader();

    glfwTerminate();
    return 0;
}