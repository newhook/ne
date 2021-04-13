/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

namespace wc {

	class CreepComponent : public artemis::Component
	{
	public:
		bool dead;
		float health;

		CreepComponent()
		{
			dead = false;
		}
	};
}