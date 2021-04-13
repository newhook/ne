#include <artemis/Config.h>
#include <artemis/DelayedEntityProcessingSystem.h>

using namespace artemis;

void DelayedEntityProcessingSystem::processEntities(const std::vector<Entity*> & entities, float accumulatedDelta) {
	for(auto p = entities.begin(); p != entities.end(); ++p) {
		processEntity(*p, accumulatedDelta);
	}
}
