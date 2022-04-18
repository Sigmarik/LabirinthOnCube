#include "Camera.h"


Camera::Camera(int cameraType) {
	type = cameraType;
}

glm::mat4 Camera::matrix() {
	glm::mat4 toView(1.0);
	glm::mat4 toSSpace(1.0);
	toView = glm::lookAt(position, position + forward, up);
	if (type == CAMERA_PERSPECTIVE) {
		toSSpace = glm::perspective(fov, (float)displayWidth / (float)displayHeight, nearPlane, farPlane);
	}
	else if (type == CAMERA_ORTHOGRAPHIC) {
		//toSSpace = glm::ortho(-orthoWidth / 2.0, orthoWidth / 2.0, -orthoHeight / 2.0, orthoHeight / 2.0);
		toSSpace = glm::mat4(
			glm::vec4(1.0, 0.0, 0.0, 0.0),
			glm::vec4(0.0, 1.0, 0.0, 0.0),
			glm::vec4(0.0, 0.0, -1.0 / farPlane, 0.0),
			glm::vec4(0.0, 0.0, 0.0, 1.0)
		);
	}
	return toSSpace * toView;
}

void Camera::pushMatrices(Shader& shader, bool shadowCamera) {
	glm::mat4 worldToSSpace = matrix();
	if (!shadowCamera) {
		glUniformMatrix4fv(shader.uniform("cameraMatrix"), 1, GL_FALSE, glm::value_ptr(worldToSSpace));
		glUniform1f(shader.uniform("nearPlane"), (GLfloat)nearPlane);
		glUniform1f(shader.uniform("farPlane"), (GLfloat)farPlane);
		glUniform3f(shader.uniform("cameraPosition"), position.x, position.y, position.z);
		glUniform3f(shader.uniform("cameraDirection"), forward.x, forward.y, forward.z);
		glUniform3f(shader.uniform("cameraUp"), up.x, up.y, up.z);
		glUniform1f(shader.uniform("cameraFOV"), fov);
		glUniform1f(shader.uniform("cameraWH"), (float)displayWidth / (float)displayHeight);
	}
	else {
		glUniformMatrix4fv(shader.uniform("shadowMatrix"), 1, GL_FALSE, glm::value_ptr(worldToSSpace));
	}
}

void Camera::operateInputs(GLFWwindow* window, double deltaTime) {
	/*
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += glm::normalize(forward) * (float)(speed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= glm::normalize(forward) * (float)(speed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= glm::normalize(glm::cross(forward, up)) * (float)(speed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += glm::normalize(glm::cross(forward, up)) * (float)(speed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position -= glm::normalize(up) * (float)(speed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position += glm::normalize(up) * (float)(speed * deltaTime);
	}
	*/
	if (enabledRotation) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glm::vec3 sideVector = glm::normalize(glm::cross(targetUp, targetForward));
		float pitch = glm::radians(mouseY - displayHeight / 2) * sencitivity;
		float yaw = -glm::radians(mouseX - displayWidth / 2) * sencitivity;
		targetUp = glm::normalize(glm::cross(targetForward, sideVector));
		targetForward = glm::rotate(targetForward, yaw, targetUp);
		targetForward = glm::rotate(targetForward, pitch, sideVector);
		//up = localUp();
		glfwSetCursorPos(window, displayWidth / 2, displayHeight / 2);
	}
	if (cinematic) {
		forward += (targetForward - forward) * armStrength * (float)deltaTime;
		up += (targetUp - up) * armStrength * (float)deltaTime;
	}
	else {
		forward = targetForward;
		up = targetUp;
	}
	forward = normalize(forward);
	up = normalize(up);
	position = -forward * armLength;
	if (glfwGetMouseButton(window, 1) == GLFW_PRESS) {
		enabledRotation = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwSetCursorPos(window, displayWidth / 2, displayHeight / 2); 
	}
	if (glfwGetMouseButton(window, 1) == GLFW_RELEASE) {
		enabledRotation = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

glm::vec3 Camera::localRight() {
	return glm::normalize(glm::cross(up, forward));
}

glm::vec3 Camera::localUp() {
	return glm::normalize(glm::cross(forward, localRight()));
}