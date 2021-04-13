/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <SFML/System/InputStream.hpp>

namespace ge {
class VsShader {
	int _shaderId;

public:

	VsShader();
	virtual ~VsShader();

	bool load(sf::InputStream&);

	int getShaderId() const {
		return _shaderId;
	}
};
}