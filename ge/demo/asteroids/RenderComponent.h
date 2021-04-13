/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>
#include <artemis/Component.h>

#include <ge/Renderable.h>
#include <ge/MeshR.h>

#include <vector>
#include <memory>

namespace asteroids {

	class RenderComponent : public artemis::Component
	{
	public:
		std::vector<std::shared_ptr<ge::Renderable> > renderable;

		RenderComponent(const std::shared_ptr<ge::Renderable>& renderable)
		{
			this->renderable.push_back(renderable);
		}

		RenderComponent(const std::vector<std::shared_ptr<ge::Renderable> >& renderable)
		{
			this->renderable = renderable;
		}

		RenderComponent(const std::vector<std::shared_ptr<ge::MeshR> >& renderable)
		{
			for (auto p = renderable.begin(); p != renderable.end(); ++p) {
				this->renderable.push_back(*p);
			}
		}
	};
}