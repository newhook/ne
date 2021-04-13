/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>
#include <artemis/Artemis.h>

#include <ge/Transform.h>

namespace wc {

	class EntityFactory : public artemis::Manager {

	public:

		EntityFactory();

		virtual void initialize();

		std::shared_ptr<artemis::Entity> createCreep(const glm::vec3& position, int team);
		std::shared_ptr<artemis::Entity> createTower(const glm::vec3& position, int team);
		std::shared_ptr<artemis::Entity> createUniverse();
		std::shared_ptr<artemis::Entity> createPlayer(const glm::vec3& position, int team);
		std::shared_ptr<artemis::Entity> createShot(const std::shared_ptr<artemis::Entity>& source, const std::shared_ptr<artemis::Entity>& target);
		std::shared_ptr<artemis::Entity> createExplosion(const glm::vec3& position);
		std::shared_ptr<artemis::Entity> createPick(const glm::vec3& position);

	};
}
