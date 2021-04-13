/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <ge/Bounds.h>

namespace wc {

	class ObstructionComponent : public artemis::Component
	{
	public:

		bool enabled;
		ge::Bounds bounds;

		ObstructionComponent()
		{
			enabled = true;
		}
	};
}