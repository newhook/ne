/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <vector>

namespace ge {

class Cubemap {

	GLuint _texture;

public:

	// TODO: Assignment/copy constructor.
	Cubemap();
	virtual ~Cubemap();

	// NEGATIVE_X
	// POSITIVE_X
	// NEGATIVE_Y
	// POSITIVE_Y
	// NEGATIVE_Z
	// POSITIVE_Z
	bool load(const std::vector<std::string>& textures);

	GLuint getTextureId() const {
		return _texture;
	}
};

}