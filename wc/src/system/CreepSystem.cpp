/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/CreepSystem.h>
#include <component/CreepComponent.h>
#include <component/ExpireComponent.h>
#include <EntityFactory.h>

using namespace wc;
using namespace std;

CreepSystem::CreepSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<CreepComponent>()), _time(5.0f) {
}

void CreepSystem::initialize() {
}

void CreepSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	CreepComponent* creep = creepMapper.get(e);
	if (creep->dead) {
		return;
	}

	PathComponent* path = pathMapper.get(e);
	// If things change or we cannot move to the next location then we
	// need to re-path.
	if (path->atgoal) {
		creep->dead = true;
		e->addComponent(make_unique<ExpireComponent>(1.0f));
		e->changedInWorld();
	}

}

void CreepSystem::begin() {
	//_time -= world->getDelta();

	if (_time <= 0) {
		_time = 5.0f;
		//glm::vec3 start = glm::vec3(0, 0, -128);
		glm::vec3 start = glm::vec3(0, 0, -32.0f);

		shared_ptr<artemis::Entity> creepEntity = world->getManager<EntityFactory>()->createCreep(start, 1);
		creepEntity->addToWorld();

		CreepComponent* creep = creepMapper.get(creepEntity);
		creep->health = 100.0f;

		PathComponent* path = pathMapper.get(creepEntity);
		path->speed = 5.0f;
		path->setGoal(glm::vec3(0, 0, 32.0f));
	}
}

void CreepSystem::end() {

}