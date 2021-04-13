/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Cubemap.h>
#include <ge/InputStream.h>
#include <ge/Logger.h>

#include <SFML/Graphics.hpp>

using namespace ge;
using namespace std;

namespace {
Logger logger(__FILE__);
}

Cubemap::Cubemap() : _texture(0) {

}

Cubemap::~Cubemap() {
	if (_texture != 0) {
		glDeleteTextures(1, &_texture);
	}
}

// TODO: Flip flags?
bool
Cubemap::load(const std::vector<std::string>& names) {
	if (names.size() != 6) {
		return false;
	}

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

	static GLuint order[] = {
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z };

	for (size_t i = 0; i < 6; ++i) {
		ge::FileInputStream stream;
		if (!stream.open(names[i])) {
			LOG_W("cannot open %s", names[i]);
			continue;
		}

		sf::Image image;
		if (!image.loadFromStream(stream))
		{
			return false;
		}
		image.flipHorizontally();
		glTexImage2D(order[i], 0, GL_RGBA8, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}
