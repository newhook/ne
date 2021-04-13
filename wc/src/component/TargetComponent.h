/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>
#include <artemis/Artemis.h>

namespace wc {

	class TargetComponent : public artemis::Component
	{
	public:

		int team;

		TargetComponent(int team)
		{
			this->team = team;
		}
	};
}