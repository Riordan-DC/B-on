
/////TEST STRUCT//////
struct VERT_NORM{
    glm::vec3 vert;
    glm::vec3 norm;
    VERT_NORM(glm::vec3 v, glm::vec3 n){
        vert = v;
        norm = n;
    }
};

Shader mShader = Shader("shaders/TransformVertexShader.vert", "shaders/TextureFragmentShader.frag");
//Shader mShader = Shader("StandardShading.vertexshader", "StandardShading.fragmentshader" );



// Read our .obj file
std::vector<unsigned short> indices;
std::vector< glm::vec3 > vertices;
//std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals; // Won't be used at the moment.
//Warning, loadAssImp will load all the models pieces into differently indexed places. Each must be rendered individually
//otherwise only the first one will be rendered. I.E. USe a VAO

std::cout << vertices.size() << std::endl;

std::vector<VERT_NORM> data_test;
for(int i = 0; i<vertices.size(); i++)
{
    VERT_NORM datum(vertices[i], normals[i]);
    data_test.push_back(datum);
}

VertexArray va;
VertexBuffer vb(&data_test[0],  vertices.size() * sizeof(glm::vec3) * 2);
//VertexBuffer nb(&normals[0],  normals.size() * sizeof(glm::vec3));
//IndexBuffer ib(&indices[0], indices.size());


VertexBufferLayout layout;
layout.Push_FLOAT(3);
layout.Push_FLOAT(3);
va.AddBuffer(vb, layout);
//va.AddBuffer(nb, layout);

/* Buffer structure: [position, normal]
 * Both position and normal are sizeof(glm::vec3).
 * This means position is offset by 0 and
 * normal is offset by sizeof(glm::vec3).
 */
/*
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2, NULL);
glEnableVertexAttribArray(0);

// normal attribute
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2, (void*)(sizeof(glm::vec3)));
glEnableVertexAttribArray(1);

*/

mShader.use();
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//Plane plane;
//GLuint LightID = glGetUniformLocation(mShader.ID, "LightPosition_worldspace");


// Game Loop //
while (glfwWindowShouldClose(window) == false && running) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    getDeltaTime();
    updateController(window, deltaTime);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    mShader.use();
    updateCamera(mShader);

    //plane.Draw(mShader);

    glm::mat4 model(1.0);
    mShader.setMat4("M", model);

    //glm::vec3 lightPos = glm::vec3(4,4,4);
    //mShader.setVec3("LightPosition_worldspace", lightPos.x, lightPos.y, lightPos.z);

    va.Bind();
    
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
    */

    /////////////////////////////
    mShader.use();
    updateCamera(mShader);

    //plane.Draw(mShader);

    glm::mat4 model2(1.0);
    model2 = glm::translate(model2, glm::vec3(5.0,0.0,0.0));
    mShader.setMat4("M", model2);

    //glm::vec3 lightPos = glm::vec3(4,4,4);
    //mShader.setVec3("LightPosition_worldspace", lightPos.x, lightPos.y, lightPos.z);
    crysis.Draw(mShader);
    //va.Bind();
    
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 2);
    //glBindVertexArray(0);
    ////////////////////////////////
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// optional: de-allocate all resources once they've outlived their purpose:
//glDeleteVertexArrays(1, &VAO);
//glDeleteBuffers(1, &VBO);
//glDeleteBuffers(1, &EBO);

cleanup();
return 0;

