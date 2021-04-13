/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <string>

namespace ge {

class ShaderAttribute {
	const std::string _name;
	const int _variableId;

public:

	ShaderAttribute(const std::string& name, int location);
	virtual ~ShaderAttribute();

	int getIndex() const {
		return _variableId;
	}
};
}