//World Object Class
#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <btBulletDynamicsCommon.h>

#include "Model.hpp"
#include "Renderer.hpp"


class Object {
	public:
		glm::mat4 ModelMatrix;
		glm::vec3 Position;
		glm::quat Orientation;
		glm::mat4 Scale;
		std::string name;

		unsigned int entity_tag;

		float mass;

		bool visable = true;
		bool selected = false;

		Object(unsigned int entity_tag);
		Object(Model model, unsigned int entity_tag);
		Object(std::string path, unsigned int entity_tag);
		~Object(void);

		void Update(float deltaTime);
		void RenderObject(Renderer &render);
		void SetPosition(glm::vec3 pos);
		void Translate(glm::vec3 pos);
		void ApplyForce(glm::vec3 force, glm::vec3 rel_pos);
		btRigidBody* InitPhysics(btDiscreteDynamicsWorld* dynamicsWorld);
		void SetScale(glm::vec3 scale);
		void AddShader(std::string name, Shader _shader);
		void Selected(bool selected);
		Shader* GetShader(std::string name);

	private:
		Model* ObjectModel;
		btCollisionShape* boxCollisionShape;
		btRigidBody* rigidBody;
		btDefaultMotionState* motionstate;

		std::map<std::string, Shader*> shaders;
		Shader shader;

		void LoadModel(std::string const &path);
};

#endif
