/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/ExpireComponent.h>

namespace wc {

	class ExpirationSystem : public artemis::EntityProcessingSystem
	{
		artemis::ComponentMapper<ExpireComponent> mapper;

	public:

		ExpirationSystem();

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
	};
}