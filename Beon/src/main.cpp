// Local Headers
#include "beon.hpp"
#include "Shader.hpp"
#include "CameraController.hpp"
#include "Object.hpp"
#include "Render.hpp"
#include "Model.hpp"

static bool running = true;

//forward declaration of functions
void cleanup();

WindowManager* Manager = WindowManager::getInstance();

int main(int argc, char* argv[])
{
    if(Manager->initWindow("Beon", mWidth, mHeight) == -1){
        std::cout << "Window failed to initialize." << std::endl;
        return -1;
    };

    GLFWwindow* window = Manager->getWindow();
    InitController(window, mWidth, mHeight);

    Render MainView(&camera, mWidth, mHeight);

    Shader mShader = Shader("../Beon/shaders/CodeMaterialShader.vert", "../Beon/shaders/CodeMaterialShader.frag");
    Shader mCubmap = Shader("../Beon/shaders/CubeMap.vert", "../Beon/shaders/CubeMap.frag" );

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

    int box_count = 10;

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


    for(int i = 0; i < box_count; i++){
        objects[i]->mass = 10.0;
        objects[i]->InitPhysics(dynamicsWorld);
        objects[i]->SetPosition(glm::vec3(0.0,(i*5)+10.0,0.0));
        objects[i]->ApplyForce(glm::vec3(rand()%300, rand()%300, rand()%300), glm::vec3(0.f,0.f,0.f));
    }

    // Game Loop //
    while (glfwWindowShouldClose(window) == false && running) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        getDeltaTime();
        updateController(window, deltaTime);
        dynamicsWorld->stepSimulation(
			deltaTime,						// Time since last step
			1,								// Mas substep count
			btScalar(1.) / btScalar(60.));	// Fixed time step 


        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MainView.Update();
        MainView.UpdateShader(mShader);
        
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0);
        lightColor.y = sin(glfwGetTime() * 0.7);
        lightColor.z = sin(glfwGetTime() * 1.3);

        mShader.setVec3("dirLight.direction", lightColor);
        mShader.setVec3("dirLight.ambient", lightColor);
        mShader.setVec3("dirLight.diffuse", lightColor);
        mShader.setVec3("dirLight.specular", lightColor);

        // spotLight
        mShader.setBool("spotLight.On", true);
        mShader.setVec3("spotLight.position", camera.Position);
        mShader.setVec3("spotLight.direction", camera.Front);
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
	
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	// de-allocate bullet physics
	delete dynamicsWorld;
	
	delete solver;
	delete broadphase;
	delete dispatcher;

	delete collisionConfiguration;
	
	

    cleanup();
    return 0;
}

void cleanup()
{
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
	
	delete Manager;
}


