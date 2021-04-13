/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/PhysicsComponent.h>
#include <component/TransformComponent.h>

class btDiscreteDynamicsWorld;
class btDynamicsWorld;

namespace wc {

class PhysicsSystem : public artemis::EntityProcessingSystem
{
	btDiscreteDynamicsWorld* _world;
	artemis::ComponentMapper<PhysicsComponent> physicsMapper;
	artemis::ComponentMapper<TransformComponent> transformMapper;

public:

	PhysicsSystem(btDiscreteDynamicsWorld* world);

	virtual void initialize();

	virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);

	virtual void inserted(const std::shared_ptr<artemis::Entity>& e);
	virtual void removed(const std::shared_ptr<artemis::Entity>& e);

	btDynamicsWorld* getPhysicsWorld();
};

}