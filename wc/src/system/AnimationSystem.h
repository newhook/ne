/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/AnimationComponent.h>

namespace wc {

	class AnimationSystem : public artemis::EntityProcessingSystem
	{
		artemis::ComponentMapper<AnimationComponent> mapper;

	public:

		AnimationSystem();

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
	};
}