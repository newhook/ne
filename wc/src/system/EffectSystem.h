/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/EntityProcessingSystem.h>
#include <artemis/ComponentMapper.h>
#include <artemis/Entity.h>

#include <component/ScaleComponent.h>
#include <component/RenderComponent.h>
#include <component/TransformComponent.h>

namespace wc {
	class EffectSystem : public artemis::EntityProcessingSystem {
		artemis::ComponentMapper<ScaleComponent> scaleMapper;
		artemis::ComponentMapper<FlipbookComponent> fbMapper;
		artemis::ComponentMapper<BillboardComponent> bbMapper;
		artemis::ComponentMapper<RenderComponent> renderMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;
		
	public:

		EffectSystem();

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
	};
}