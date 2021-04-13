/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/ProjectileComponent.h>
#include <component/TransformComponent.h>

namespace wc {
	class ProjectileSystem : public artemis::EntityProcessingSystem {
		artemis::ComponentMapper<ProjectileComponent> projectileMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;
		
	public:

		ProjectileSystem();

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
	};
}