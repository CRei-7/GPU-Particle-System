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

void CreateShaders() {
	Shader* shaderProgram = new Shader();
	shaderProgram->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shaderProgram);
}

int main(){
	mainWindow = Window(SCR_WIDTH, SCR_HEIGHT);
	mainWindow.initialize();

    CreateShaders();

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
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

        glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getxChange(), mainWindow.getyChange());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();

		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();

		//glUniformMatrix4fv binds a uniform to the shader, in this case we are binding the projection and view matrices to the shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));//This is for sending the projection matrix to the shader, 1 is for count, GL_FALSE is for whether we want to transpose the matrix or not, value_ptr is for converting the matrix to a pointer
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		mainWindow.swapBuffers();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
	shaderList[0].ClearShader();

    glfwTerminate();
    return 0;
}