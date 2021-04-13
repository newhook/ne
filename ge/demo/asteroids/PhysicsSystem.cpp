/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/PhysicsSystem.h>

#include <btBulletDynamicsCommon.h>

using namespace asteroids;

PhysicsSystem::PhysicsSystem(btDiscreteDynamicsWorld* world) : _world(world) {
	setComponentTypes<PhysicsComponent>();
}

void 
PhysicsSystem::initialize() {
	physicsMapper.init(*world);
	transformMapper.init(*world);
}

void 
PhysicsSystem::processEntity(artemis::Entity &e) {
	TransformComponent* transform = transformMapper.get(e);
	if (transform) {
		PhysicsComponent* physics = physicsMapper.get(e);
		btTransform trans;
		physics->rigid->getMotionState()->getWorldTransform(trans);
		transform->transform = trans;
	}
}

void 
PhysicsSystem::added(artemis::Entity &e) {
	PhysicsComponent* physics = physicsMapper.get(e);
	if (physics->collisionGroup == 0 && physics->collisionMask == 0) {
		_world->addRigidBody(physics->rigid);
	}
	else {
		_world->addRigidBody(physics->rigid, physics->collisionGroup, physics->collisionMask);
	}
}

void
PhysicsSystem::removed(artemis::Entity &e) {
	PhysicsComponent* physics = physicsMapper.get(e);
	_world->removeRigidBody(physics->rigid);
}

btDynamicsWorld*
PhysicsSystem::getPhysicsWorld() {
	return _world;
}