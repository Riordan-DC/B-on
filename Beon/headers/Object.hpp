//World Object Class
#ifndef OBJECT_H
#define OBJECT_H
#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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
		std::string name;

		float mass;
		Object(){}
		Object(Model model){
			this->ObjectModel = model.Clone();
			this->ModelMatrix = glm::mat4(1.0);
			this->Position = glm::vec3(0.0, 0.0, 0.0);
			this->boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
			this->Orientation = glm::quat(glm::vec3(0, 45, 0));
			this->mass = 1.0;
		}
		Object(std::string path){
			this->LoadModel(path);
			this->ModelMatrix = glm::mat4(1.0);
			this->Position = glm::vec3(0.0, 0.0, 0.0);
			this->boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
			this->Orientation = glm::quat(glm::vec3(0, 45, 0));
			this->mass = 1.0;
		}

		void Update(float deltaTime){
			btQuaternion q = this->rigidBody->getOrientation();
			float qx = q.x();
			float qy = q.y();
			float qz = q.z();
			float qw = q.w();

			const float n = 1.0f/sqrt(qx*qx+qy*qy+qz*qz+qw*qw);
			qx *= n;
			qy *= n;
			qz *= n;
			qw *= n;

			this->Orientation = glm::quat(q.x(), q.y(), q.z(), q.w());
			glm::mat4 RotationMatrix = glm::mat4(0.0);

			RotationMatrix[0][0] = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
			RotationMatrix[0][1] = 2.0f*qx*qy - 2.0f*qz*qw;
			RotationMatrix[0][2] = 2.0f*qx*qz + 2.0f*qy*qw;
			RotationMatrix[0][3] = 0.0f;
			RotationMatrix[1][0] = 2.0f*qx*qy + 2.0f*qz*qw;
			RotationMatrix[1][1] = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
			RotationMatrix[1][2] = 2.0f*qy*qz - 2.0f*qx*qw;
			RotationMatrix[1][3] = 0.0f;
			RotationMatrix[2][0] = 2.0f*qx*qz - 2.0f*qy*qw;
			RotationMatrix[2][1] = 2.0f*qy*qz + 2.0f*qx*qw;
			RotationMatrix[2][2] = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;
			RotationMatrix[2][3] = 0.0f;
			RotationMatrix[3][0] = 0.0f;
			RotationMatrix[3][1] = 0.0f;
			RotationMatrix[3][2] = 0.0f;
			RotationMatrix[3][3] = 1.0f;

			this->Position = glm::vec3(this->rigidBody->getCenterOfMassPosition().x(), this->rigidBody->getCenterOfMassPosition().y(), this->rigidBody->getCenterOfMassPosition().z());
			this->ModelMatrix = glm::translate(glm::mat4(1.0), this->Position) * glm::toMat4(this->Orientation);
			//printf("%f %f %f\n", this->rigidBody->getCenterOfMassPosition().x(), this->rigidBody->getCenterOfMassPosition().y(), this->rigidBody->getCenterOfMassPosition().z());
		}

		void Render(Render &render){
			render.UpdateShader(this->shader);
			this->shader.use();
			this->shader.setMat4("Model", this->ModelMatrix);
			this->ObjectModel->Draw(this->shader);
		}

		void SetPosition(glm::vec3 pos){
			btTransform	transform(btQuaternion(), btVector3(btScalar(pos.x), btScalar(pos.y), btScalar(pos.z)));
			this->rigidBody->setCenterOfMassTransform(transform);
			
		}

		void Translate(glm::vec3 pos){
			this->rigidBody->translate(btVector3(pos.x, pos.y, pos.z));
		}

		void ApplyForce(glm::vec3 force, glm::vec3 rel_pos){
			this->rigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(force.x, force.y, force.z));
		}

		void InitPhysics(btDiscreteDynamicsWorld* dynamicsWorld){
			this->motionstate = new btDefaultMotionState(btTransform(
			btQuaternion(this->Orientation.x, this->Orientation.y, this->Orientation.z, this->Orientation.w), 
			btVector3(this->Position.x, this->Position.y, this->Position.z)));
			btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, motionstate, this->boxCollisionShape,btVector3(1.0,1.0,1.0));
			this->rigidBody = new btRigidBody(rigidBodyCI);

			dynamicsWorld->addRigidBody(rigidBody);

		    btTransform tr;
		    tr.setIdentity();
		    btQuaternion quat;
		    quat.setEuler(31,74,10); //or quat.setEulerZYX depending on the ordering you want
		    tr.setRotation(quat);
		    this->rigidBody->setCenterOfMassTransform(tr);
		}

	private:
		Model* ObjectModel;
		btCollisionShape* boxCollisionShape;
		btRigidBody* rigidBody;
		btDefaultMotionState* motionstate;

		void LoadModel(std::string const &path){
			this->ObjectModel = new Model(path, false);
		}

};

	// Small hack : store the mesh's index "i" in Bullet's User Pointer.
	// Will be used to know which object is picked. 
	// A real program would probably pass a "MyGameObjectPointer" instead.
	//rigidBody->setUserPointer((void*)this);

#endif
