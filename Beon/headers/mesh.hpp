#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

/* 
	Assimp uses its only structure for managing material properties.
	I am using glm to parse values to my shaders. So each draw call
	I would have to convert from aiColor3D or whatever to the
	appropriate type. This is slow so Im going to make a struct for
	my mesh variables. Currently this material struct only contains
	color values and no texture information.
*/
struct Material {
	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec3 specular;

	float shininess;

	bool hasNormalTexture;
	bool hasDiffuseTexture;
	bool hasSpecularTexture;
	bool hasHeightTexture;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

	bool has_material;
	aiMaterial* material = NULL;
	Material material_color;

    /*  Functions  */
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
		this->has_material = false;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, aiMaterial* material)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->has_material = true;
		this->material = material;

		aiString mat_name;

		// The generic way
		if (AI_SUCCESS != material->Get(AI_MATKEY_NAME, mat_name)) {
			// handle epic failure here
			std::cout << "\t[-] No material" << mat_name.C_Str() << std::endl;
		}
		else {
			std::cout << "\tMaterial : " << mat_name.C_Str() << std::endl;
		}

		// Convert assimp material property types. This way I dont have to convert each frame.
		// material variables
		aiColor3D ambient;
		aiColor3D diffuse;
		aiColor3D specular;
		float shininess;

		// get material values
		this->material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		this->material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		this->material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		this->material->Get(AI_MATKEY_SHININESS, shininess);

		this->material_color = {
			glm::vec3(ambient.r, ambient.g, ambient.b),
			glm::vec3(diffuse.r, diffuse.g, diffuse.b),
			glm::vec3(specular.r, specular.g, specular.b),
			shininess,
			false,
			false,
			false,
			false
		};
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

    // render the mesh
    void Draw(Shader shader) 
    {
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			// now set the sampler to the correct texture unit

			//glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			glUniform1i(glGetUniformLocation(shader.ID, ("material." + name).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

        // set shader material color
		// Consider setting a drawn flag. Only call these once. 
		if (this->has_material) {
			shader.setVec3("material.ambient", this->material_color.ambient);
			shader.setVec3("material.diffuse", this->material_color.diffuse);
			shader.setVec3("material.specular", this->material_color.specular);
			shader.setFloat("material.shininess", this->material_color.shininess);
		}
		shader.setBool("material.hasNormalTexture", this->material_color.hasNormalTexture);
		shader.setBool("material.hasDiffuseTexture", this->material_color.hasDiffuseTexture);
		shader.setBool("material.hasSpecularTexture", this->material_color.hasSpecularTexture);
		shader.setBool("material.hasHeightTexture", this->material_color.hasHeightTexture);

		//shader.setVec3("material.ambient", glm::vec3(0.5,0.5,0.5));
		//shader.setVec3("material.diffuse", glm::vec3(0.5,0.5,0.5));
		//shader.setVec3("material.specular", glm::vec3(0.5,0.5,0.5));
		//shader.setFloat("material.shininess", 0.1f);
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    /*  Render data  */
    unsigned int VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh()
    {
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			string name = textures[i].type;
			if (name == "texture_diffuse")
				this->material_color.hasDiffuseTexture = true;
			else if (name == "texture_specular")
				this->material_color.hasSpecularTexture = true;
			else if (name == "texture_normal")
				this->material_color.hasNormalTexture = true;
			else if (name == "texture_height")
				this->material_color.hasHeightTexture = true;
		}

        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
#endif