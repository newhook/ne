/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/CollisionSystem.h>

#include <btBulletDynamicsCommon.h>

#include <asteroids/TransformComponent.h>

using namespace asteroids;
using namespace std;

CollisionSystem::CollisionSystem(btCollisionDispatcher* dispatcher) : _dispatcher(dispatcher) {
}

void  CollisionSystem::initialize() {
}

bool  CollisionSystem::checkProcessing() {
	return true;
}

void CollisionSystem::begin() {
	int numManifolds = _dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = _dispatcher->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++) {
			artemis::Entity* entity0 = (artemis::Entity*)obA->getCollisionShape()->getUserPointer();
			artemis::Entity* entity1 = (artemis::Entity*)obB->getCollisionShape()->getUserPointer();
			if (entity0 != 0 && entity1 != 0) {
				_pairs.push_back(make_pair(entity0, entity1));
			}
		}

		contactManifold->clearManifold();
	}
}

void CollisionSystem::end() {
	_pairs.clear();
}

void CollisionSystem::processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
	for (auto p = _pairs.begin(); p != _pairs.end(); ++p) {
		if ((world->getGroupManager()->getGroupOf(*p->first) == "WORLD" &&
			world->getGroupManager()->getGroupOf(*p->second) == "BOX") ||
			(world->getGroupManager()->getGroupOf(*p->first) == "BOX" &&
			world->getGroupManager()->getGroupOf(*p->second) == "WORLD")) {

			artemis::Entity* box;
			if (world->getGroupManager()->getGroupOf(*p->first) == "BOX") {
				box = p->first;
			}
			else {
				box = p->second;
			}

			//createExplosion(world->getEntityManager(), box->getComponent<TransformComponent>()->transform.getTranslation());
			world->deleteEntity(*box);
		}
	}
}
