#include <artemis/Config.h>
#include <artemis/EntityProcessingSystem.h>

using namespace artemis;
	
bool EntityProcessingSystem::checkProcessing() {
	return true;
}

void EntityProcessingSystem::processEntities(const std::vector<std::shared_ptr<Entity> > & entities) {
	for (auto p : entities) {
		processEntity(p);
	}
}