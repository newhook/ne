#pragma once

#include <artemis/Config.h>
#include <artemis/DelayedEntitySystem.h>

namespace artemis{
	class DelayedEntityProcessingSystem : public DelayedEntitySystem
	{
		protected:

			virtual void processEntities(const std::vector<Entity*>& entities, float accumulatedDelta);
			virtual void processEntity(Entity* e, float accumulatedDelta) = 0;
	};
}