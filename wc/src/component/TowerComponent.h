/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>
#include <artemis/Artemis.h>

namespace wc {

	class TowerComponent : public artemis::Component
	{
	public:

		float time;
		float range;
		std::shared_ptr<artemis::Entity> target;

		TowerComponent()
		{
			time = 0.0f;
			range = 50.0f;
		}
	};
}