/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/ExpirationSystem.h>

using namespace asteroids;
using namespace std;

ExpirationSystem::ExpirationSystem() {
	setComponentTypes<ExpireComponent>();
}

void ExpirationSystem::initialize() {
	mapper.init(*world);
}

void  ExpirationSystem::processEntity(artemis::Entity &e) {
	ExpireComponent* component = mapper.get(e);
	component->time -= world->getDelta();
	if (component->time < 0) {
		printf("deleting %d\n", e.getId());
		world->deleteEntity(e);
	}
}
