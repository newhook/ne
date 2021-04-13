/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/ExpirationSystem.h>

using namespace wc;
using namespace std;

ExpirationSystem::ExpirationSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<ExpireComponent>()) {
}

void ExpirationSystem::initialize() {
}

void ExpirationSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	ExpireComponent* component = mapper.get(e);
	component->time -= world->getDelta();
	if (component->time < 0) {
		printf("deleting %d\n", e->getId());
		e->deleteFromWorld();
	}
}
