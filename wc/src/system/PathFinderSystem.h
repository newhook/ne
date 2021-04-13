/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/ObstructionComponent.h>
#include <component/RenderComponent.h>

#include <set>

namespace wc {


	class TerrainComponent;
	class PathFinderSystem : public artemis::EntityProcessingSystem
	{
		artemis::ComponentMapper<ObstructionComponent> mapper;
		artemis::ComponentMapper<RenderComponent> renderMapper;

		std::set<std::shared_ptr<artemis::Entity> > _entities;
		
	public:

		PathFinderSystem();

		std::vector<glm::vec3> solve(const glm::vec3& from, const glm::vec3& to);

		void setTerrain(const std::shared_ptr<artemis::Entity>& terrain);
		virtual void initialize();
		virtual void begin();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);
		virtual void inserted(const std::shared_ptr<artemis::Entity>& e);
		virtual void removed(const std::shared_ptr<artemis::Entity>& e);
	};

}