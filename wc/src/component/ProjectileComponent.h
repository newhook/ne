/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>
#include <artemis/Artemis.h>

namespace wc {

	class ProjectileComponent : public artemis::Component
	{
	public:

		float speed;
		float time;
		float endTime;
		glm::vec3 start;
		glm::vec3 end;
		
		ProjectileComponent(const glm::vec3& start, const glm::vec3& end)
		{
			this->time = 0.0f;
			this->speed = 10.0f;
			this->start = start;
			this->end = end;
			this->endTime = glm::distance(start, end) / speed;
		}
	};
}