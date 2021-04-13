/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>

#include <ge/MeshR.h>

#include <vector>
#include <memory>

namespace asteroids {
	namespace resource {
		std::vector<std::shared_ptr<ge::MeshR> > loadobj(const std::string& name, const std::string& shader);

	}
}