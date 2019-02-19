#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.hpp"

unsigned int SCR_WIDTH;
unsigned int SCR_HEIGHT;
float lastX;
float lastY;
bool firstMouse = true;
Camera camera(glm::vec3(-0.178233, -0.471922, 28.753202));


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateController(GLFWwindow* window, float deltaTime);
void processInput(GLFWwindow *window, float deltaTime);
void updateCamera(Shader *shader);

void updateCamera(Shader &shader)
{
    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
    shader.setMat4("Projection", ProjectionMatrix);

    // camera/view transformation
    glm::mat4 ViewMatrix = camera.GetViewMatrix();
    shader.setMat4("View", ViewMatrix);

    //Model transform
    //glm::mat4 ModelMatrix = glm::mat4(1.0);
    //shader.setMat4("M", ModelMatrix);

    //glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    //shader.setMat4("MVP", MVP);

    shader.setVec3("cameraPos", camera.Position);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void updateController(GLFWwindow* window, float deltaTime){
    processInput(window, deltaTime);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void InitController(GLFWwindow* window, int screenWidth, int screenHeight){

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    SCR_WIDTH = screenWidth;
    SCR_HEIGHT = screenHeight;
    lastX = SCR_HEIGHT / 2.0f;
    lastY = SCR_WIDTH / 2.0f;
}

#endif
