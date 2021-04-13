/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Uniform.h>

#include <string>

namespace ge {

class ShaderUniform {

	const std::string _name;
	const UniformType _type;

	int _variableId;

public:

	ShaderUniform(const std::string& name, UniformType type, int variableId);
	virtual ~ShaderUniform();

	int getIndex() const {
		return _variableId;
	}

	const UniformType getType() const {
		return _type;
	}
};

}