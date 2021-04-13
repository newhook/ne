/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/AnimationSystem.h>

using namespace wc;
using namespace std;

AnimationSystem::AnimationSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<AnimationComponent>()) {
}

void AnimationSystem::initialize() {
}

void AnimationSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	AnimationComponent* component = mapper.get(e);
	component->animator->update(world->getDelta());
	component->skinnedMeshR->setBones(component->animator->getPose()->getBones());
}
