
#include <vector>

// Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Shader data structures
/*
struct Material {
	ImVec4 ambient;
	ImVec4 diffuse;
	ImVec4 specular;
	float shininess;
};

struct DirLight {
	bool On;

	ImVec4 direction;

	ImVec4 ambient;
	ImVec4 diffuse;
	ImVec4 specular;
};

struct PointLight {
	bool On;

	ImVec4 position;

	float constant;
	float linear;
	float quadratic;

	ImVec4 ambient;
	ImVec4 diffuse;
	ImVec4 specular;
};
*/

namespace GUI {
	// GUI stuff
	void InitGui(GLFWwindow* window);
	void getFrame(void);
	void renderGui(void);
	void killGui(void);
	bool show_demo_window = false;
	bool show_another_window = false;

	ImVec4 backgroundColor = ImVec4(0, 0, 0, 0);

	ImVec4 DirLightAmbientColor = ImVec4(1.f, 1.f, 1.f, 0.f);
	ImVec4 DirLightDirection = ImVec4(0.f, -1.f, 0.f, 0.f);
	ImVec4 DirLightDiffuse = ImVec4(1.f, 1.f, 1.f, 0.f);
	float DirLightSpecular = 0.5f;
	float DirLightShininess = 32.f;
}

void GUI::InitGui(GLFWwindow* window) {
	// Initalise Imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void GUI::getFrame(void) {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::renderGui(void) {
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Beon Editor");

		ImGui::Text("Beon");
		//ImGui::Checkbox("Bool example", &GUI::show_demo_window);

		ImGui::ColorEdit3("Background color", (float*)&GUI::backgroundColor);
		ImGui::SliderFloat("Global shininess", &GUI::DirLightShininess, 0.0f, 100.0f);
		ImGui::ColorEdit3("Global ambient", (float*)&GUI::DirLightAmbientColor);
		ImGui::SliderFloat3("Global direction", (float*)&GUI::DirLightDirection, -255.0f, 255.0f);
		ImGui::SliderFloat3("Global diffuse", (float*)&GUI::DirLightDiffuse, 0.f, 10.0f);
		ImGui::SliderFloat("Global specular", (float*)&GUI::DirLightSpecular, 0.f, 100.0f);

		if (ImGui::Button("Button"))
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	if (GUI::show_another_window)
	{
		ImGui::Begin("Another Window", &GUI::show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			GUI::show_another_window = false;
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::killGui(void) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}