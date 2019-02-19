#ifndef RENDERABLE_H
#define RENDERABLE_H

#pragma once

#include <map>
#include <string>

#include "Shader.hpp"

// This needs to be virtual or abstract. We dont want another object in our Object class?

class Renderable{
public:
	bool visable;
	std::map<std::string, Shader*> shaders;
	Shader shader;

	Renderable(){
		this->visable = true;
		this->shader = Shader();
	}

	void AddShader(std::string name, Shader _shader){
		this->shaders[name] = &_shader;
		this->shader = _shader;
	}

	Shader* GetShader(std::string name){
		return this->shaders[name];
	}
};
#endif