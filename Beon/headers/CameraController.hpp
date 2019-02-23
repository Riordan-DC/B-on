#ifndef CAMERA_CONTROLLER
#define CAMERA_CONTROLLER

#pragma once

#include <GLFW/glfw3.h>

#include "Camera.hpp"

class CameraController {
public:
	CameraController(GLFWwindow* window, glm::vec3 cameraPosition);
	~CameraController();

	Camera* camera;

	void Update(double deltaTime);

	GLFWwindow* window;
	double xpos, ypos;
	bool firstMouse;
	double lastX, lastY;
	bool trackMouse;

};

#endif