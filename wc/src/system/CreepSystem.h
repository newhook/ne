/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/PathComponent.h>
#include <component/CreepComponent.h>
#include <component/TransformComponent.h>

namespace wc {
	class EnityFactory;
	class CreepSystem : public artemis::EntityProcessingSystem
	{
		float _time;
		artemis::ComponentMapper<PathComponent> pathMapper;
		artemis::ComponentMapper<CreepComponent> creepMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;
		EnityFactory* _factory;

	public:

		CreepSystem();

		virtual void initialize();

		virtual void begin();
		virtual void end();
		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
	};
}