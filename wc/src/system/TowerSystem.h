/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/TowerComponent.h>
#include <component/TargetComponent.h>
#include <component/TransformComponent.h>

namespace wc {
	class TowerSystem : public artemis::EntityProcessingSystem {
		artemis::ComponentMapper<TowerComponent> towerMapper;
		artemis::ComponentMapper<TargetComponent> targetMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;

		std::pair<std::unordered_multimap <std::string, std::shared_ptr<artemis::Entity> >::const_iterator,
			std::unordered_multimap <std::string, std::shared_ptr<artemis::Entity> >::const_iterator> _targets;

	public:

		TowerSystem();

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
		virtual void begin();
	};
}