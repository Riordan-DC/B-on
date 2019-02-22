// Local Headers
#include "Beon.hpp"

// Window parameters
int windowWidth = 1980 / 2;
int windowHeight = 1080 / 2;

static bool running = true;

// Forward declaration of functions
// GLFW Call back functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void toggle_mouse_tracking(GLFWwindow* window, int key, int scancode, int action, int mods);

void cleanup();

// GUI stuff
inline void GUI(void);
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 lightColor = ImVec4(0, 0, 0, 0);

// Create window manager
WindowManager* Manager = WindowManager::getInstance();

int main(int argc, char* argv[])
{
    if(Manager->initWindow("Beon", windowWidth, windowHeight) == -1){
        std::cout << "Window failed to initialize." << std::endl;
        return -1;
    };

	// Get window from window manager
	GLFWwindow* window = Manager->getWindow();

	// Initalise camera controls
	CameraController controlled_cam(window);

	// Create render view with camera
	Render MainView(controlled_cam.camera, windowWidth, windowHeight);

	// Set GLFW call backs
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, toggle_mouse_tracking);

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

	// Load shaders
    Shader mShader = Shader("../Beon/shaders/CodeMaterialShader.vert", "../Beon/shaders/CodeMaterialShader.frag");
    Shader mCubmap = Shader("../Beon/shaders/CubeMap.vert", "../Beon/shaders/CubeMap.frag" );

	// Load models
    Model cube(GetCurrentWorkingDir()+"/../Beon/assets/models/cube.obj", false);
    Model skybox;
    skybox.LoadSkyBox(GetCurrentWorkingDir()+"/../Beon/assets/skybox");
    mShader.use();
    mShader.setInt("skybox", 0);

    mShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    mShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    mShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
    mShader.setFloat("material.shininess", 32.0f);

    mShader.setBool("dirLight.On", true);

    mCubmap.use();
    mCubmap.setInt("skybox", 0);

    Object crysis(cube);
    crysis.AddShader("basic", mShader);

    Object* static_cube = new Object(cube);
	static_cube->AddShader("basic", mShader);

	int floor_width = 10;
	int floor_length = 10;
    int box_count = floor_width * floor_length;

    std::vector<Object*> objects;
    for(int i = 0; i < box_count; i++){
        Object* obj = new Object(cube);
        obj->name = to_string(i);
        obj->AddShader("basic", mShader);

        objects.push_back(obj);
    }
    mShader.use();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Get a handle on our light
    //GLuint LightID = glGetUniformLocation(mShader.ID, "LightPosition_worldspace");


   // Build the broadphase
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
 
    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
 
    // The actual physics solver
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
 
    // The world.
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,-5.1f,0));

    crysis.mass = 10.0;
    crysis.InitPhysics(dynamicsWorld);
    crysis.SetPosition(glm::vec3(0.0,10.0,0.0));
    crysis.ApplyForce(glm::vec3(0.0,30.0,0.0), glm::vec3(0.0,5.0,0.0));
	static_cube->mass = 0.0;
	static_cube->InitPhysics(dynamicsWorld);
	static_cube->SetPosition(glm::vec3(0.0, -10.0,0.0));

    for(int i = 0; i < floor_width; i++){
		for (int j = 0; j < floor_length; j++) {
			int index = (i*floor_length) + j;
			objects[index]->mass = 0.0;
			objects[index]->InitPhysics(dynamicsWorld);
			objects[index]->SetPosition(glm::vec3((j * 3), 0.0, (i * 3)));
		}
    }

    // Game Loop //
    while (glfwWindowShouldClose(window) == false && running) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        getDeltaTime();
		controlled_cam.Update(deltaTime);
        dynamicsWorld->stepSimulation(
			deltaTime,						// Time since last step
			1,								// Mas substep count
			btScalar(1.) / btScalar(60.));	// Fixed time step 


        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MainView.Update(deltaTime);
        MainView.UpdateShader(mShader);
        
        
        //lightColor.x = sin(glfwGetTime() * 2.0);
        //lightColor.y = sin(glfwGetTime() * 0.7);
        //lightColor.z = sin(glfwGetTime() * 1.3);

        mShader.setVec3("dirLight.direction", glm::vec3(lightColor.x, lightColor.y, lightColor.z));
        mShader.setVec3("dirLight.ambient", glm::vec3(lightColor.x, lightColor.y, lightColor.z));
        mShader.setVec3("dirLight.diffuse", glm::vec3(lightColor.x, lightColor.y, lightColor.z));
        mShader.setVec3("dirLight.specular", glm::vec3(lightColor.x, lightColor.y, lightColor.z));

        // spotLight
        mShader.setBool("spotLight.On", true);
        mShader.setVec3("spotLight.position", controlled_cam.camera->Position);
        mShader.setVec3("spotLight.direction", controlled_cam.camera->Front);
        mShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        mShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        mShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        mShader.setFloat("spotLight.constant", 1.0f);
        mShader.setFloat("spotLight.linear", 0.09f);
        mShader.setFloat("spotLight.quadratic", 0.032f);
        mShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        mShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));   
        
        crysis.Update(deltaTime);
        crysis.RenderObject(MainView);

        for(int i = 0; i < box_count; i++){
            objects[i]->Update(deltaTime);
            objects[i]->RenderObject(MainView);
        }

		static_cube->Update(deltaTime);
		static_cube->RenderObject(MainView);

        glDepthFunc(GL_LEQUAL);
        MainView.UpdateShader(mCubmap);
        //skybox.DrawSkyBox(mShader);
        skybox.DrawSkyBox(mCubmap);

		GUI();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources once they've outlived their purpose:
	for (int i = 0; i < box_count; i++)
	{
		delete objects[i];
	}
	delete static_cube;
	


	// de-allocate bullet physics
	
	//for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	//{
	//	btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
	//	btRigidBody* body = btRigidBody::upcast(obj);
	//	if (body && body->getMotionState()) {
	//		delete body->getMotionState();
	//	}
	//	dynamicsWorld->removeCollisionObject(obj);
	//	delete obj;
	//}
	//delete dynamicsWorld;
	//delete solver;
	//delete broadphase;
	//delete dispatcher;
	//delete collisionConfiguration;
	

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


    cleanup();
    return 0;
}

void cleanup()
{
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
	
	delete Manager;
}

inline void GUI(void) {
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&lightColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}