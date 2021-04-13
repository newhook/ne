/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>

#include <artemis/Component.h>

namespace asteroids {
	class ExpireComponent : public artemis::Component {
	public:

		ExpireComponent(float t) {
			time = t;
		}

		float time;
	};
}