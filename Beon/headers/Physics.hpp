#include <btBulletDynamicsCommon.h>

struct BulletSystem {
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
};

BulletSystem StartBulletPhysics(void) {
	// Build the broadphase
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// The world.
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	BulletSystem bullet = { broadphase , collisionConfiguration , dispatcher , solver , dynamicsWorld };

	return bullet;
}

void DestroyBulletPhysics(BulletSystem* bulletSystem) {
	// de-allocate bullet physics
	/*
	for (int i = bulletSystem->dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		btCollisionObject* obj = bulletSystem->dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		bulletSystem->dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	
	delete bulletSystem->dynamicsWorld;
	delete bulletSystem->solver;
	delete bulletSystem->broadphase;
	delete bulletSystem->dispatcher;
	delete bulletSystem->collisionConfiguration;
	*/
}