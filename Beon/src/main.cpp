// Local Headers
#include "beon.hpp"
#include "Shader.hpp"
#include "CameraController.hpp"
#include "Object.cpp"
#include "Render.hpp"
#include "Model.hpp"

static bool running = true;

//forward declaration of functions
void cleanup();
void render(const VertexBuffer& vb, const IndexBuffer& ib, Shader& shader);

WindowManager* Manager = WindowManager::getInstance();

int main()
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

    Object plane(cube);
    plane.AddShader("basic", mShader);

    int box_count = 100;

    std::vector<Object*> objects;
    for(int i = 0; i < box_count; i++){
        Object* obj = new Object(cube);
        obj->name = to_string(i);
        obj->AddShader("basic", mShader);

        objects.push_back(obj);
    }
    mShader.use();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //Plane plane;
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
    plane.mass = 0.0;
    plane.InitPhysics(dynamicsWorld);
    plane.SetPosition(glm::vec3(0.0, -10.0,0.0));


    for(int i = 0; i < box_count; i++){
        objects[i]->mass = 10.0;
        objects[i]->InitPhysics(dynamicsWorld);
        objects[i]->SetPosition(glm::vec3(0.0,(i*5)+10.0,0.0));
        objects[i]->ApplyForce(glm::vec3(rand()%300, rand()%300, rand()%300), glm::vec3(rand()%20, rand()%20, rand()%20));
        //printf("OBJECT %s SET!\n", objects[i]->name.c_str());
    }

    // Game Loop //
    while (glfwWindowShouldClose(window) == false && running) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        getDeltaTime();
        updateController(window, deltaTime);
        dynamicsWorld->stepSimulation(1.0/60.0);


        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        MainView.Update();
        MainView.UpdateShader(mShader);
        
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        mShader.setVec3("dirLight.direction", lightColor);
        mShader.setVec3("dirLight.ambient", lightColor);
        mShader.setVec3("dirLight.diffuse", lightColor);
        mShader.setVec3("dirLight.specular", lightColor);
        // point light 1
        /*
        mShader.setVec3("pointLights[0].position", camera.Position);
        mShader.setVec3("pointLights[0].ambient", 1.f, 1.f, 1.f);
        mShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        mShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        mShader.setFloat("pointLights[0].constant", 1.0f);
        mShader.setFloat("pointLights[0].linear", 0.09);
        mShader.setFloat("pointLights[0].quadratic", 0.032);
        */
        // spotLight
        mShader.setBool("spotLight.On", true);
        mShader.setVec3("spotLight.position", camera.Position);
        mShader.setVec3("spotLight.direction", camera.Front);
        mShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        mShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        mShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        mShader.setFloat("spotLight.constant", 1.0f);
        mShader.setFloat("spotLight.linear", 0.09);
        mShader.setFloat("spotLight.quadratic", 0.032);
        mShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        mShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));   
        
        /*
        // light properties
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
        mShader.setVec3("light.ambient", glm::vec3(1.0, 1.0,1.0));
        mShader.setVec3("light.diffuse", glm::vec3(1.0, 1.0,1.0));
        mShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        */

        crysis.Update(deltaTime);
        crysis.Render(MainView);

        for(int i = 0; i < box_count; i++){
            objects[i]->Update(deltaTime);
            objects[i]->Render(MainView);
        }

        plane.Update(deltaTime);
        plane.Render(MainView);

        //printf("OBJECT CRYSI POS (%f, %f, %f)\n", crysis.Position.x, crysis.Position.y, crysis.Position.z);
        //printf("OBJECT %s AT POS (%f, %f, %f)\n", objects[0]->name.c_str(), objects[0]->Position.x, objects[0]->Position.y, objects[0]->Position.z);


        //glm::vec3 lightPos = glm::vec3(4,4,4);
        //mShader.setVec3("LightPosition_worldspace", lightPos.x, lightPos.y, lightPos.z);
        //crysis.Draw(mShader);

        glDepthFunc(GL_LEQUAL);
        MainView.UpdateShader(mCubmap);
        //skybox.DrawSkyBox(mShader);
        skybox.DrawSkyBox(mCubmap);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
	for (int i = 0; i < box_count; i++)
	{
		delete objects[i];
	}

    cleanup();
    return 0;
}

//void render(const VertexBuffer& vb, const IndexBuffer& ib, Shader& shader){
//
//}

void cleanup()
{
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}


