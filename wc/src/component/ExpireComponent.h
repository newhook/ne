/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

namespace wc {
	class ExpireComponent : public artemis::Component {
	public:

		ExpireComponent(float t) {
			time = t;
		}

		float time;
	};
}