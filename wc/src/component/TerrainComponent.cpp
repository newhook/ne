/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <Config.h>
#include <component/TerrainComponent.h>

using namespace wc;

float
TerrainComponent::getHeight(glm::vec3 pos) const {
	pos += extends;

	if (pos.x < 0 || pos.x > heightmap->getSize().x - 1 || pos.z < 0 || pos.z > heightmap->getSize().y - 1) {
		return 0.0f;
	}

	sf::Color c = heightmap->getPixel((int)pos.x, (int)pos.z);
	return (c.r / 256.0f) * bias;
}
