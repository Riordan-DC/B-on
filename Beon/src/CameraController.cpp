#include "CameraController.hpp"

CameraController::CameraController(GLFWwindow* window) {
	this->window = window;
	this->camera = new Camera(glm::vec3(-0.178233, -0.471922, 28.753202));
	this->trackMouse = true;
}

CameraController::~CameraController() {
	delete this->camera;
}

void CameraController::Update(double deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		this->camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		this->camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		this->camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		this->camera->ProcessKeyboard(RIGHT, deltaTime);

	//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	//	trackMouse = !trackMouse;

	if (this->trackMouse) {
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
}