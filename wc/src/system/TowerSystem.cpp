/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <Config.h>
#include <system/TowerSystem.h>

#include <EntityFactory.h>

#include <ge/Logger.h>

using namespace wc;
using namespace std;

namespace {
ge::Logger logger(__FILE__);
}

TowerSystem::TowerSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<TowerComponent>()){
}

void TowerSystem::initialize() {
}


void TowerSystem::begin() {
	_targets = world->getManager<artemis::GroupManager>()->getEntities("target");
}

void TowerSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	TowerComponent* tower = towerMapper.get(e);
	TargetComponent* towerTarget = targetMapper.get(e);
	TransformComponent* towerTransform = transformMapper.get(e);

	if (_targets.first != _targets.second) {
		TowerComponent* tower = towerMapper.get(e);	
		TargetComponent* towerTarget = targetMapper.get(e);
		TransformComponent* towerTransform = transformMapper.get(e);

		glm::vec3 towerPos = towerTransform->getWorldTransform().getTranslation();
		for (auto p = _targets.first; p != _targets.second; ++p)  {
			const shared_ptr<artemis::Entity> targetEntity = p->second;

			TargetComponent* target = targetMapper.get(targetEntity);
			if (towerTarget->team == target->team) {
				continue;
			}

			TransformComponent* targetTransform = transformMapper.get(targetEntity);

			glm::vec3 targetPos = targetTransform->getWorldTransform().getTranslation();
			if (glm::distance(towerPos, targetPos) < tower->range) {
				if (tower->target != targetEntity) {
					tower->target = targetEntity;
				}

				float damping = 1.0f;

				// Find the turret.
				auto turret = find(towerTransform->names.begin(), towerTransform->names.end(), "Cannon");
				if (turret != towerTransform->names.end()) {
					size_t ti = turret - towerTransform->names.begin();
					glm::quat rotation = ge::lookRotation(
						targetTransform->getWorldTransform().getTranslation() -
						towerTransform->getWorldTransform(ti).getTranslation(), 
						glm::vec3(0, 1, 0));

					glm::quat swing, twist;
					ge::swingTwist(rotation, glm::vec3(0, 1, 0), swing, twist);

					rotation = glm::slerp(towerTransform->localTransform[ti].getRotation(),
						twist, world->getDelta() * damping);

					towerTransform->localTransform[ti].setRotation(rotation);
				}

				if (tower->time > 0) {
					tower->time -= world->getDelta();
					if (tower->time > 0) {
						return;
					}
					tower->time = 0;
				}

				tower->time = 0.5f;
				world->getManager<EntityFactory>()->createShot(e, targetEntity)->addToWorld();
			}
		}
	}
}
