/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>

#include <artemis/EntityProcessingSystem.h>
#include <artemis/ComponentMapper.h>
#include <artemis/Entity.h>

#include <asteroids/ExpireComponent.h>

namespace asteroids {

	class ExpirationSystem : public artemis::EntityProcessingSystem
	{
		artemis::ComponentMapper<ExpireComponent> mapper;

	public:

		ExpirationSystem();

		virtual void initialize();

		virtual void processEntity(artemis::Entity &e);
	};
}