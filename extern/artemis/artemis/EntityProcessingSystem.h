#pragma once

#include <artemis/Config.h>
#include <artemis/EntitySystem.h>

namespace artemis {

	class EntityProcessingSystem : public EntitySystem {
		protected:

			EntityProcessingSystem(const Aspect& aspect) : EntitySystem(aspect) {}
			virtual void processEntity(const std::shared_ptr<artemis::Entity>& e) = 0;
			void processEntities(const std::vector<std::shared_ptr<Entity> > & entities);
			virtual bool checkProcessing();
	};
}