#include "Camera.h"

Camera::Camera() {

}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed) {
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;

	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;
	//firstMouseMove = true;
	right = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 0.0f, 0.0f);

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime) {

	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W]) {
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_D]) {
		position += right * velocity;
	}
	if (keys[GLFW_KEY_A]) {
		position -= right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {
	//if (firstMouseMove) {
	//	firstMouseMove = false;
	//	return;  // Ignore the first mouse input
	//}

	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f) {//so the scene won't turn upside down, this is purely for adding constraint
		pitch = 89.0f;
	}

	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix() {
	return glm::lookAt(position, position + front, up);//(position, target, up of Camera)
}

glm::vec3 Camera::getCameraPosition() {
	return position;
}

glm::vec3 Camera::getCameraDirection() {
	return glm::normalize(front);
}

void Camera::update() {
	glm::vec3 tempFront;
	tempFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tempFront.y = sin(glm::radians(pitch));
	tempFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(tempFront);

	right = glm::normalize(glm::cross(front, worldUp));//since front is pointing towards -Z and up towards +Y, cross product is +X i.e. right
	up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera() {

}
