#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include "camera.hpp"
#include "Shader.hpp"

class Render{
public:
	Camera* camera;
	int viewWidth;
	int viewHeight;

	Render(Camera *camera, int viewWidth, int viewHeight){
		this->camera = camera;
		this->viewWidth = viewWidth;
		this->viewHeight = viewHeight;
	}
	
	void Update(void){
		this->camera->UpdateCamera(this->viewWidth, this->viewHeight);
	}

	/*
	void Draw(Object &object){
		this->UpdateShader(object.shader);
		object.Render();
	}
	*/

	void UpdateShader(Shader shader){
		shader.use();

        shader.setMat4("Projection", this->camera->ProjectionMatrix);

        shader.setMat4("View", this->camera->ViewMatrix);

        shader.setVec3("cameraPos", this->camera->Position);
	}

	void UpdateShader(Shader shader, glm::mat4 &ModelMatrix){
		shader.use();
		
		shader.setMat4("Projection", this->camera->ProjectionMatrix);

		shader.setMat4("View", this->camera->ViewMatrix);

		shader.setMat4("Model", ModelMatrix);

        glm::mat4 MVP = this->camera->ProjectionMatrix * this->camera->ViewMatrix * ModelMatrix;
        shader.setMat4("MVP", MVP);

        shader.setVec3("cameraPos", this->camera->Position);
	}

};
#endif