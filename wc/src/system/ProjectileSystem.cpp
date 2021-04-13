/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/ProjectileSystem.h>

#include <EntityFactory.h>

using namespace wc;
using namespace std;

ProjectileSystem::ProjectileSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<ProjectileComponent>()) {
}

void ProjectileSystem::initialize() {
}

void ProjectileSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	ProjectileComponent* projectile = projectileMapper.get(e);
	TransformComponent* transform = transformMapper.get(e);

	projectile->time += world->getDelta();
	if (projectile->time > projectile->endTime) {
		world->getManager<EntityFactory>()->createExplosion(projectile->end)->addToWorld();
		// TODO: deduct damage from creep and so forth.
		e->deleteFromWorld();
		return;
	}

	glm::vec3 pos = glm::mix(projectile->start, projectile->end, projectile->time / projectile->endTime);

	transform->transform.setTranslation(pos);
}
