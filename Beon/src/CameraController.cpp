#include "CameraController.hpp"

CameraController::CameraController(GLFWwindow* window, glm::vec3 cameraPosition) {
	this->window = window;
	this->camera = new Camera(cameraPosition);
	this->trackMouse = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

CameraController::~CameraController() {
	delete this->camera;
}

void CameraController::Update(double deltaTime) {
	if (this->trackMouse) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			this->camera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			this->camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			this->camera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			this->camera->ProcessKeyboard(RIGHT, deltaTime);

		glfwGetCursorPos(this->window, &this->xpos, &this->ypos);

		if (this->firstMouse)
		{
			this->lastX = this->xpos;
			this->lastY = this->ypos;
			this->firstMouse = false;
		}

		float xoffset = this->xpos - this->lastX;
		float yoffset = this->lastY - this->ypos; // reversed since y-coordinates go from bottom to top

		this->lastX = this->xpos;
		this->lastY = this->ypos;

		this->camera->ProcessMouseMovement(xoffset, yoffset);
	}
	else {
		this->firstMouse = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}