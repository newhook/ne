/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/ShaderUniform.h>

using namespace ge;
using namespace std;



ShaderUniform::ShaderUniform(const string& name, UniformType type, int variableId) :
	_name(name),
	_type(type),
	_variableId(variableId) {
}

ShaderUniform::~ShaderUniform() {

}

