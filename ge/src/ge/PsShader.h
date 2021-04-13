/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <SFML/System/InputStream.hpp>

namespace ge {
class PsShader {
	int _shaderId;

public:

	PsShader();
	virtual ~PsShader();

	bool load(sf::InputStream&);

	int getShaderId() const {
		return _shaderId;
	}
};
}