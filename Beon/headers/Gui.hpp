
#include <vector>

// Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GUI {
	// GUI stuff
	void InitGui(GLFWwindow* window);
	void getFrame(void);
	void renderGui(void);
	void killGui(void);
	bool show_demo_window = false;
	bool show_another_window = false;
	static void GUILoadMenu();
	static void Misc();

	ImVec4 backgroundColor = ImVec4(0, 0, 0, 0);

	ImVec4 DirLightAmbientColor = ImVec4(1.f, 1.f, 1.f, 0.f);
	ImVec4 DirLightDirection = ImVec4(0.f, -1.f, 0.f, 0.f);
	float DirLightDiffuse = 1.0f;
	float DirLightSpecular = 0.5f;
	float DirLightShininess = 32.f;

	bool p_open;

	Object* selected_object = NULL;
	ImVec4 selected_object_pos = ImVec4(0, 0, 0, 0);
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
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_MenuBar;

		ImGui::Begin("Beon Editor", &GUI::p_open, window_flags);

		ImGui::Text("Beon (v0.1)");
		// Menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Dev"))
			{
				ImGui::MenuItem("Load", NULL, &GUI::GUILoadMenu);
				ImGui::MenuItem("Console", NULL, &GUI::GUILoadMenu);
				ImGui::MenuItem("Log", NULL, &GUI::GUILoadMenu);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::CollapsingHeader("Entity Catalog")) {
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("Catalog of Entities");
		}
		if (ImGui::CollapsingHeader("Entity Managment")) {
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("Entity Managment");
			ImGui::Text("With border:");
			ImGui::Columns(4, "mycolumns"); // 4-ways, with border
			ImGui::Separator();
			ImGui::Text("ID"); ImGui::NextColumn();
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Path"); ImGui::NextColumn();
			ImGui::Text("Hovered"); ImGui::NextColumn();
			ImGui::Separator();
			const char* names[3] = { "One", "Two", "Three" };
			const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
			static int selected = -1;
			for (int i = 0; i < 3; i++)
			{
				char label[32];
				sprintf(label, "%04d", i);
				if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
					selected = i;
				bool hovered = ImGui::IsItemHovered();
				ImGui::NextColumn();
				ImGui::Text(names[i]); ImGui::NextColumn();
				ImGui::Text(paths[i]); ImGui::NextColumn();
				ImGui::Text("%d", hovered); ImGui::NextColumn();
			}
			ImGui::Columns(1);
		}
		if (ImGui::CollapsingHeader("Misc")) {
			ImGuiIO& io = ImGui::GetIO();
			GUI::Misc();
		}

		ImGui::End();
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void GUI::killGui(void) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

static void GUI::GUILoadMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Load"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

static void GUI::Misc()
{
	{
		static float f = 0.0f;

		ImGui::ShowDemoWindow();

		ImGui::ColorEdit3("Background color", (float*)&GUI::backgroundColor);
		ImGui::SliderFloat("Global shininess", &GUI::DirLightShininess, 0.0f, 128.0f);
		ImGui::ColorEdit3("Global ambient", (float*)&GUI::DirLightAmbientColor);
		ImGui::SliderFloat3("Global direction", (float*)&GUI::DirLightDirection, -1.0f, 1.0f);
		ImGui::SliderFloat("Global diffuse", &GUI::DirLightDiffuse, 0.f, 10.0f);
		ImGui::SliderFloat("Global specular", &GUI::DirLightSpecular, 0.f, 10.0f);

		if (GUI::selected_object) {
			char buffer[128];
			sprintf(buffer, "Obj[%d] position:", GUI::selected_object->entity_tag);
			GUI::selected_object_pos.x = GUI::selected_object->Position.x;
			GUI::selected_object_pos.y = GUI::selected_object->Position.y;
			GUI::selected_object_pos.z = GUI::selected_object->Position.z;
			ImGui::InputFloat3(buffer, (float*)&GUI::selected_object_pos);
			GUI::selected_object->SetPosition(glm::vec3(
				GUI::selected_object_pos.x,
				GUI::selected_object_pos.y,
				GUI::selected_object_pos.z));
		}
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (GUI::show_another_window)
	{
		ImGui::Begin("Another Window", &GUI::show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			GUI::show_another_window = false;
	}
}


