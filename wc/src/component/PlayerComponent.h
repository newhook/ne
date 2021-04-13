/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>
#include <artemis/Artemis.h>

namespace wc {

	class PlayerComponent : public artemis::Component
	{
	public:

		bool rightDown;
		bool leftDown;

		PlayerComponent()
		{
			rightDown = false;
			leftDown = false;
		}
	};
}