/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>
#include <artemis/World.h>
#include <artemis/Entity.h>

#include <ge/Transform.h>

namespace asteroids {

	class EntityFactory {
		artemis::World* _world;

	public:
		EntityFactory(artemis::World* world);

		artemis::Entity& createUniverse();
		artemis::Entity& createPlayer(const glm::vec3& position);
		artemis::Entity& createAsteroid(const glm::vec3& position);
	};
}
