/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/PathSystem.h>
#include <system/PathFinderSystem.h>
#include <component/TerrainComponent.h>
#include <EntityFactory.h>

using namespace wc;
using namespace std;

PathSystem::PathSystem() : EntityProcessingSystem(artemis::Aspect::getAspectFor<PathComponent, TransformComponent>()), _time(5.0f) {
}

void PathSystem::initialize() {
}

void PathSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	PathComponent* path = pathMapper.get(e);
	if (path->stopped) {
		return;
	}

	TransformComponent* transform = transformMapper.get(e);

	// If things change or we cannot move to the next location then we
	// need to re-path.

	PathFinderSystem* pathFinderSystem = world->getSystem<PathFinderSystem>();
	shared_ptr<artemis::Entity> terrainEntity = world->getManager<artemis::TagManager>()->getEntity("terrain");
	TerrainComponent* terrain = terrainEntity->getComponent<TerrainComponent>();

	glm::vec3 pos = transform->transform.getTranslation();

	if (path->path.size() == 0) {
		ObstructionComponent* obstruction = obstructionMapper.get(e);
		bool enabled = false;
		// Dislike this.
		if (obstruction) {
			std::swap(enabled, obstruction->enabled);
		}

		path->path = pathFinderSystem->solve(pos, path->goal);

		if (obstruction) {
			std::swap(enabled, obstruction->enabled);
		}

		if (path->path.empty()) {
			return;
		}

		//printf("f=(%d, %d) t=(%d, %d) l=%d ", (int)pos.x, (int)pos.z, (int)path->goal.x, (int)path->goal.z, path->path.size());
		//for (size_t i = 0; i < path->path.size(); ++i) {
		//	if (i != 0) {
		//		printf(", ");
		//	}
		//	printf("(%.2f, %.2f)", path->path[i].x, path->path[i].z);
		//}

		path->segment = 0;
		path->segmentLength = glm::distance(path->path[path->segment], path->path[path->segment + 1]);
		path->distOnSegment = 0;
		return;
	}

	path->distOnSegment += path->speed * world->getDelta();
	if (path->distOnSegment > path->segmentLength) {
		if (path->segment == path->path.size() - 2) {
			path->stopped = true;
			path->atgoal = true;
			return;
		}

		path->segment++;
		path->segmentLength = glm::distance(path->path[path->segment], path->path[path->segment + 1]);
		path->distOnSegment = 0;
	}

	pos = glm::mix(path->path[path->segment], path->path[path->segment +1], path->distOnSegment / path->segmentLength);
	pos.y = terrain->getHeight(pos);

	transform->transform.setTranslation(pos);
}
