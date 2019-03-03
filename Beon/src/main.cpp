#include "Beon.hpp"

// Window parameters
int windowWidth = 1980;
int windowHeight = 1080;

static bool running = true;

//Get a handle on our light
//GLuint LightID = glGetUniformLocation(mShader.ID, "LightPosition_worldspace");

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
	Renderer MainView(controlled_cam.camera, windowWidth, windowHeight);

	// Initalise Gui
	GUI::InitGui(window);

	// Load shaders
	Shader core_shader = Shader("../Beon/shaders/TransformVertexShader.vert", "../Beon/shaders/TextureFragmentShader.frag");
	//Shader crysis_shader = Shader("../Beon/shaders/Toon.vert", "../Beon/shaders/Toon.frag");
	Shader mCubmap = Shader("../Beon/shaders/CubeMap.vert", "../Beon/shaders/CubeMap.frag" );

	// Load models
    //Model monkey_model(GetCurrentWorkingDir()+"/../Beon/assets/models/suzanne/suzanne.obj", false);
	//Model man_model(GetCurrentWorkingDir() + "/../Beon/assets/models/people/Male_Casual.obj", false);
	//Model crysis_model(GetCurrentWorkingDir() + "/../Beon/assets/models/nanosuit/nanosuit.obj", false);
	Model cube_model(GetCurrentWorkingDir() + "/../Beon/assets/models/cube/cube.obj", false);


    Model skybox;
    skybox.LoadSkyBox(GetCurrentWorkingDir()+"/../Beon/assets/skybox");

    mCubmap.use();
    mCubmap.setInt("skybox", 0);

    Object* crysis = new Object(cube_model, 0);
    crysis->AddShader("texture", core_shader);

    Object* monkey = new Object(cube_model, 1);
	monkey->AddShader("basic", core_shader);

	Object* man = new Object(cube_model, 562);
	man->AddShader("material", core_shader);

	Object* floor = new Object(cube_model, 30000);
	floor->AddShader("material", core_shader);

    //crysis_shader.use();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	BulletSystem bulletSystem = StartBulletPhysics();

	bulletSystem.dynamicsWorld->setGravity(btVector3(0,-5.1,0));

    crysis->mass = 1.0;
    crysis->InitPhysics(bulletSystem.dynamicsWorld);
    crysis->SetPosition(glm::vec3(0.0,40.0,0.0));
	man->mass = 1.0;
	man->InitPhysics(bulletSystem.dynamicsWorld);
	man->SetPosition(glm::vec3(0.0, 70.0,0.0));
	monkey->mass = 1.0;
	monkey->InitPhysics(bulletSystem.dynamicsWorld);
	monkey->SetPosition(glm::vec3(0.f, 90.0, 0.0));
	floor->mass = 0.0;
	floor->InitPhysics(bulletSystem.dynamicsWorld);
	floor->SetPosition(glm::vec3(0.0, 0.0, 0.0));
	floor->SetScale(glm::vec3(1,10,10));

	double lastTime = glfwGetTime();
    // Game Loop //
    while (glfwWindowShouldClose(window) == false && running) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

		getDeltaTime();

		// UPDATE
		bulletSystem.dynamicsWorld->stepSimulation(
			deltaTime,						// Time since last step
			7,								// Mas substep count
			btScalar(1.) / btScalar(60.));	// Fixed time step 

		crysis->Update(deltaTime);
		man->Update(deltaTime);
		monkey->Update(deltaTime);
		floor->Update(deltaTime);


		// RENDER
		GUI::getFrame();

        glClearColor(GUI::backgroundColor.x, GUI::backgroundColor.y, GUI::backgroundColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MainView.Update();

		MainView.UpdateShader(core_shader);

		core_shader.setBool("dirLight.On", true);
		core_shader.setVec3("dirLight.direction", glm::vec3(GUI::DirLightDirection.x, GUI::DirLightDirection.y, GUI::DirLightDirection.z));
		core_shader.setVec3("dirLight.ambient", glm::vec3(GUI::DirLightAmbientColor.x, GUI::DirLightAmbientColor.y, GUI::DirLightAmbientColor.z));
		core_shader.setVec3("dirLight.diffuse", glm::vec3(GUI::DirLightDiffuse, GUI::DirLightDiffuse, GUI::DirLightDiffuse));
		core_shader.setVec3("dirLight.specular", glm::vec3(GUI::DirLightSpecular, GUI::DirLightSpecular, GUI::DirLightSpecular));
		core_shader.setFloat("dirLight.shininess", GUI::DirLightShininess);

		core_shader.setBool("pointLights[0].On", false);
		core_shader.setVec3("pointLights[0].position", controlled_cam.camera->Position);
		core_shader.setVec3("pointLights[0].ambient", glm::vec3(GUI::DirLightAmbientColor.x, GUI::DirLightAmbientColor.y, GUI::DirLightAmbientColor.z));
		core_shader.setVec3("pointLights[0].specular", glm::vec3(GUI::DirLightSpecular, GUI::DirLightSpecular, GUI::DirLightSpecular));
		core_shader.setVec3("pointLights[0].diffuse", glm::vec3(GUI::DirLightDiffuse, GUI::DirLightDiffuse, GUI::DirLightDiffuse));
		core_shader.setFloat("pointLights[0].quadratic", 0.032f);
		core_shader.setFloat("pointLights[0].linear", 0.09f);
		core_shader.setFloat("pointLights[0].constant", 3.0f);

		crysis->RenderObject(MainView);
		man->RenderObject(MainView);
		monkey->RenderObject(MainView);
		floor->RenderObject(MainView);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {


			glm::vec3 out_origin;
			glm::vec3 out_direction;
			double xpos, ypos;
			if (controlled_cam.trackMouse) {
				xpos = windowWidth / 2;
				ypos = windowHeight / 2;
			}
			else {
				glfwGetCursorPos(window, &xpos, &ypos);
				ypos = windowHeight - ypos;
			}
			ScreenPosToWorldRay(
				(int)xpos, (int)ypos,
				windowWidth, windowHeight,
				controlled_cam.camera->ViewMatrix,
				controlled_cam.camera->ProjectionMatrix,
				out_origin,
				out_direction
			);
			glm::vec3 out_end = out_origin + out_direction * 1000.0f;

			btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z));
			bulletSystem.dynamicsWorld->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
			if (RayCallback.hasHit()) {
				if (GUI::selected_object) {
					GUI::selected_object->Selected(false);
				}
				GUI::selected_object = (Object*)RayCallback.m_collisionObject->getUserPointer();
				GUI::selected_object->Selected(true);
				//std::cout << "mesh " << GUI::selected_object->entity_tag << std::endl;
			}
			else {
				if (GUI::selected_object) {
					GUI::selected_object->Selected(false);
				}
				//std::cout << "background" << std::endl;;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
			// Escape camera mode
			GUI::fly_camera = false;

		}
		controlled_cam.Update(deltaTime);
		controlled_cam.trackMouse = GUI::fly_camera;

        glDepthFunc(GL_LEQUAL);
        MainView.UpdateShader(mCubmap);
        //skybox.DrawSkyBox(crysis_shader);
        skybox.DrawSkyBox(mCubmap);

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
	delete man;
	delete monkey;
	delete floor;
	delete crysis;

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