#ifndef LABGAME_CAMERA_CLASS_H
#define LABGAME_CAMERA_CLASS_H

#define CAMERA_PERSPECTIVE 0
#define CAMERA_ORTHOGRAPHIC 1
#define CAMERA_EMPTY 2

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_relational.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_common.hpp>
#include <glm/vector_relational.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"

class Camera {
public:
	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 forward = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 targetForward = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 targetUp = glm::vec3(0.0, 1.0, 0.0);
	bool cinematic = false;
	float armStrength = 0.1;
	float fov = 3.1415926535 * (1.5 / 4.0);
	int displayWidth = 800;
	int displayHeight = 800;
	float orthoWidth  = 1.2;
	float orthoHeight = 1.2;
	float nearPlane = 0.1;
	float farPlane = 1000.0;
	float speed = 30;
	float sencitivity = 1.0;
	float armLength = 3.5f;
	glm::vec2 lastCursorPos = glm::vec2(0.0);
	bool enabledRotation = false;
	int type = CAMERA_PERSPECTIVE;
	Camera(int cameraType = CAMERA_PERSPECTIVE);
	glm::mat4 matrix();
	void pushMatrices(Shader& shader, bool shadowCamera=false);
	void operateInputs(GLFWwindow* window, double deltaTime);
	glm::vec3 localRight();
	glm::vec3 localUp();
};

#endif