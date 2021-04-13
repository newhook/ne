/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/ShaderAttribute.h>

using namespace ge;

ShaderAttribute::ShaderAttribute(const std::string& name, int location) :
	_name(name),
	_variableId(location) {

}

ShaderAttribute::~ShaderAttribute() {

}