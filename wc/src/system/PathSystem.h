/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/PathComponent.h>
#include <component/TransformComponent.h>
#include <component/ObstructionComponent.h>

namespace wc {
	class EnityFactory;
	class PathSystem : public artemis::EntityProcessingSystem
	{
		float _time;
		artemis::ComponentMapper<PathComponent> pathMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;
		artemis::ComponentMapper<ObstructionComponent> obstructionMapper;

		EnityFactory* _factory;

	public:

		PathSystem();

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
	};
}