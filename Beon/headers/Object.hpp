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
#include "Renderable.hpp"
#include "Render.hpp"


class Object: public Renderable{
	public:
		glm::mat4 ModelMatrix;
		glm::vec3 Position;
		glm::quat Orientation;
		glm::mat4 Scale;
		std::string name;

		float mass;

		Object(void);
		Object(Model model);
		Object(std::string path);
		~Object(void);

		void Update(float deltaTime);
		void RenderObject(Render &render);
		void SetPosition(glm::vec3 pos);
		void Translate(glm::vec3 pos);
		void ApplyForce(glm::vec3 force, glm::vec3 rel_pos);
		void InitPhysics(btDiscreteDynamicsWorld* dynamicsWorld);
		void SetScale(float scaleFactor);

	private:
		Model* ObjectModel;
		btCollisionShape* boxCollisionShape;
		btRigidBody* rigidBody;
		btDefaultMotionState* motionstate;

		void LoadModel(std::string const &path);
};

	// Small hack : store the mesh's index "i" in Bullet's User Pointer.
	// Will be used to know which object is picked. 
	// A real program would probably pass a "MyGameObjectPointer" instead.
	//rigidBody->setUserPointer((void*)this);

#endif
