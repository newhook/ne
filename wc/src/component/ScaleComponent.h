/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

namespace wc {
	class ScaleComponent : public artemis::Component {
	public:

		float start;
		float end;
		float duration;

		float time;

		ScaleComponent(float start, float end, float duration) {
			this->start = start;
			this->end = end;
			this->duration = duration;
			this->time = 0.0f;
		}
	};

	class FlipbookComponent : public artemis::Component {
	public:

		int width;
		float period;
		float time;

		FlipbookComponent(int width, float period) {
			this->width = width;
			this->period = period;
			this->time = 0.0f;
		}
	};

	class BillboardComponent : public artemis::Component {
	public:
		glm::quat rot;
		BillboardComponent(const glm::quat& rot) {
			this->rot = rot;
		}
	};
}