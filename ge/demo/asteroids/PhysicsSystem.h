/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>

#include <artemis/EntityProcessingSystem.h>
#include <artemis/ComponentMapper.h>
#include <artemis/Entity.h>

#include <asteroids/PhysicsComponent.h>
#include <asteroids/TransformComponent.h>

class btDiscreteDynamicsWorld;
class btDynamicsWorld;

namespace asteroids {

class PhysicsSystem : public artemis::EntityProcessingSystem
{
	btDiscreteDynamicsWorld* _world;
	artemis::ComponentMapper<PhysicsComponent> physicsMapper;
	artemis::ComponentMapper<TransformComponent> transformMapper;

public:

	PhysicsSystem(btDiscreteDynamicsWorld* world);

	virtual void initialize();

	virtual void processEntity(artemis::Entity &e);

	virtual void added(artemis::Entity &e);
	virtual void removed(artemis::Entity &e);

	btDynamicsWorld* getPhysicsWorld();
};

}