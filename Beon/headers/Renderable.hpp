//Renderable Object
#ifndef RENDERABLE_H
#define RENDERABLE_H
#pragma once
#include <map>
#include <string>

#include "Shader.hpp"

class Renderable{
public:
	bool visable;
	std::map<std::string, Shader*> shaders;
	Shader shader;

	Renderable(){
		this->visable = true;
		this->shader = Shader();
	}

	void AddShader(std::string name, Shader shader){
		this->shaders[name] = &shader;
		this->shader = shader;
	}

	Shader* GetShader(std::string name){
		return this->shaders[name];
	}
};
#endif