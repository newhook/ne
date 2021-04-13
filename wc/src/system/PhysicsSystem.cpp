/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/PhysicsSystem.h>

#include <ge/Logger.h>
#include <btBulletDynamicsCommon.h>

using namespace wc;
namespace {
ge::Logger logger(__FILE__);
}

PhysicsSystem::PhysicsSystem(btDiscreteDynamicsWorld* world) : EntityProcessingSystem(artemis::Aspect::getAspectFor<PhysicsComponent>()), _world(world) {
}

void 
PhysicsSystem::initialize() {
}

void 
PhysicsSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	TransformComponent* transform = transformMapper.get(e);
	if (transform) {
		PhysicsComponent* physics = physicsMapper.get(e);
		btTransform trans;
		physics->rigid->getMotionState()->getWorldTransform(trans);
		transform->transform = trans;
		//LOG_O("%d: translation=%.2f,%.2f,%.2f", e->getId(), transform->transform.getTranslation().x, transform->transform.getTranslation().y, transform->transform.getTranslation().z);
	}
}

void 
PhysicsSystem::inserted(const std::shared_ptr<artemis::Entity>& e) {
	PhysicsComponent* physics = physicsMapper.get(e);
	if (physics->collisionGroup == 0 && physics->collisionMask == 0) {
		_world->addRigidBody(physics->rigid.get());
	}
	else {
		_world->addRigidBody(physics->rigid.get(), physics->collisionGroup, physics->collisionMask);
	}
}

void
PhysicsSystem::removed(const std::shared_ptr<artemis::Entity>& e) {
	PhysicsComponent* physics = physicsMapper.get(e);
	_world->removeRigidBody(physics->rigid.get());
}

btDynamicsWorld*
PhysicsSystem::getPhysicsWorld() {
	return _world;
}