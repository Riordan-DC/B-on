#include "Beon.hpp"

// Window parameters
int windowWidth = 1980 - 300;
int windowHeight = 1080 - 300;

static bool running = true;

// Forward declaration of functions
void cleanup();

// Create window manager
WindowManager* Manager = WindowManager::getInstance();

int main(int argc, char* argv[]) {
	if (Manager->initWindow("Beon", windowWidth, windowHeight) == -1) {
		std::cout << "Window failed to initialize." << std::endl;
		return -1;
	};

	// Get window from window manager
	GLFWwindow* window = Manager->getWindow();

	// Set GLFW call backs
	setBasicGLFWCallbacks(window);

	// Initalise camera controls
	CameraController controlled_cam(window, glm::vec3(0,10,10));

	// Create render view with camera
	Render MainView(controlled_cam.camera, windowWidth, windowHeight);

	// Initalise Gui
	GUI::InitGui(window);

	// Load shaders
    Shader mShader = Shader("../Beon/shaders/CodeMaterialShader.vert", "../Beon/shaders/CodeMaterialShader.frag"); // Coded colors. Use over textures.
	Shader crysis_shader = Shader("../Beon/shaders/TransformVertexShader.vert", "../Beon/shaders/TextureFragmentShader.frag"); // Textures
    Shader mCubmap = Shader("../Beon/shaders/CubeMap.vert", "../Beon/shaders/CubeMap.frag" );

	// Load models
    Model cube(GetCurrentWorkingDir()+"/../Beon/assets/models/cube.obj", true);
	Model monkey(GetCurrentWorkingDir() + "/../Beon/assets/models/OBJ/Male_Casual.obj", false);
	Model crysis_model(GetCurrentWorkingDir() + "/../Beon/assets/models/nanosuit/nanosuit.obj", false);

    //Model skybox;
    //skybox.LoadSkyBox(GetCurrentWorkingDir()+"/../Beon/assets/skybox");
    mShader.use();
    mShader.setInt("skybox", 0);

    mShader.setBool("dirLight.On", true);
	crysis_shader.use();
	crysis_shader.setBool("dirLight.On", true);

    mCubmap.use();
    mCubmap.setInt("skybox", 0);

    Object crysis(monkey);
    crysis.AddShader("texture", crysis_shader);

    Object* static_cube = new Object(crysis_model);
	static_cube->AddShader("basic", crysis_shader);

    //mShader.use();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Get a handle on our light
    //GLuint LightID = glGetUniformLocation(mShader.ID, "LightPosition_worldspace");
	
	BulletSystem bulletSystem = StartBulletPhysics();

	bulletSystem.dynamicsWorld->setGravity(btVector3(0,-5.1f,0));

    crysis.mass = 10.0;
    crysis.InitPhysics(bulletSystem.dynamicsWorld);
    crysis.SetPosition(glm::vec3(0.0,10.0,0.0));
    crysis.ApplyForce(glm::vec3(0.0,30.0,0.0), glm::vec3(0.0,5.0,0.0));

	static_cube->mass = 0.0;
	static_cube->InitPhysics(bulletSystem.dynamicsWorld);
	static_cube->SetPosition(glm::vec3(0.0, -20.0,0.0));
	static_cube->SetScale(10.f);

	double lastTime = glfwGetTime();
    // Game Loop //
    while (glfwWindowShouldClose(window) == false && running) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

		getDeltaTime();

		// UPDATE
		controlled_cam.Update(deltaTime);
		bulletSystem.dynamicsWorld->stepSimulation(
			deltaTime,						// Time since last step
			1,								// Mas substep count
			btScalar(1.) / btScalar(60.));	// Fixed time step 

		crysis.Update(deltaTime);
		static_cube->Update(deltaTime);


		// RENDER
		GUI::getFrame();

        glClearColor(GUI::backgroundColor.x, GUI::backgroundColor.y, GUI::backgroundColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MainView.Update();

		MainView.UpdateShader(crysis_shader);

		crysis_shader.setVec3("dirLight.direction", glm::vec3(GUI::DirLightDirection.x, GUI::DirLightDirection.y, GUI::DirLightDirection.z));
		crysis_shader.setVec3("dirLight.ambient", glm::vec3(GUI::DirLightAmbientColor.x, GUI::DirLightAmbientColor.y, GUI::DirLightAmbientColor.z));
		crysis_shader.setVec3("dirLight.diffuse", glm::vec3(GUI::DirLightDiffuse.x, GUI::DirLightDiffuse.y, GUI::DirLightDiffuse.z));
		crysis_shader.setVec3("dirLight.specular", glm::vec3(GUI::DirLightSpecular, GUI::DirLightSpecular, GUI::DirLightSpecular));
		crysis_shader.setFloat("dirLight.shininess", GUI::DirLightShininess);

		crysis_shader.setBool("spotLight.On", true);
		crysis_shader.setVec3("spotLight.position", controlled_cam.camera->Position);
		crysis_shader.setVec3("spotLight.direction", controlled_cam.camera->Front);
		crysis_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		crysis_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		crysis_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		crysis_shader.setFloat("spotLight.constant", 1.0f);
		crysis_shader.setFloat("spotLight.linear", 0.09f);
		crysis_shader.setFloat("spotLight.quadratic", 0.032f);
		crysis_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		crysis_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		crysis_shader.setBool("pointLights[0].On", true);
		crysis_shader.setVec3("pointLights[0].position", controlled_cam.camera->Position);
		crysis_shader.setVec3("pointLights[0].ambient", glm::vec3(GUI::DirLightAmbientColor.x, GUI::DirLightAmbientColor.y, GUI::DirLightAmbientColor.z));
		crysis_shader.setVec3("pointLights[0].specular", glm::vec3(GUI::DirLightAmbientColor.x, GUI::DirLightAmbientColor.y, GUI::DirLightAmbientColor.z));
		crysis_shader.setVec3("pointLights[0].diffuse", glm::vec3(GUI::DirLightAmbientColor.x, GUI::DirLightAmbientColor.y, GUI::DirLightAmbientColor.z));
		crysis_shader.setFloat("pointLights[0].quadratic", 0.032f);
		crysis_shader.setFloat("pointLights[0].linear", 0.09f);
		crysis_shader.setFloat("pointLights[0].constant", 1.0f);

		static_cube->RenderObject(MainView);
		crysis.RenderObject(MainView);
		

        //glDepthFunc(GL_LEQUAL);
        //MainView.UpdateShader(mCubmap);
        ////skybox.DrawSkyBox(mShader);
        //skybox.DrawSkyBox(mCubmap);

		// Render GUI
		GUI::renderGui();

		// In time left over, poll events again.
		while (glfwGetTime() < lastTime + (1.0 / 60.0)) {
			// Do nothing
			continue;
		}
		lastTime += (1.0 / 60.0);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources once they've outlived their purpose:
	delete static_cube;

	GUI::killGui();

	DestroyBulletPhysics(&bulletSystem);

    cleanup();
    return 0;
}

void cleanup() {
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

	delete Manager;
}