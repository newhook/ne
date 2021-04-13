/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

namespace wc {

	class PathComponent : public artemis::Component
	{
	public:

		bool stopped;
		bool atgoal;

		float speed;
		glm::vec3 goal;

		float segmentLength;
		float distOnSegment;
		size_t segment;
		std::vector<glm::vec3> path;

		PathComponent() {
			stopped = true;
			atgoal = false;
		}

		void setGoal(const glm::vec3& g) {
			stopped = false;
			atgoal = false;
			goal = g;
			path.clear();
		}

		void stop() {
			stopped = true;
		}

		void start() {
			stopped = false;
			path.clear();
		}

		void repath() {
			path.clear();
		}
	};
}