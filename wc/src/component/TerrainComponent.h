/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <SFML/Graphics/Image.hpp>

#include <memory>


namespace wc {
	class World;
	class TerrainComponent : public artemis::Component
	{
	public:

		bool translate;
		float bias;
		glm::vec3 extends;
		std::shared_ptr<sf::Image> heightmap;

		TerrainComponent(const std::shared_ptr<sf::Image>& heightmap)
		{
			bias = 25.0f;
			extends = glm::vec3(heightmap->getSize().x / 2.0, 0, heightmap->getSize().y / 2.0);
			this->heightmap = heightmap;
			translate = true;
		}

		float getHeight(glm::vec3 pos) const;
	};
}