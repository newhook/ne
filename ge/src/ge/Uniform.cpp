/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Uniform.h>

using namespace ge;

UniformType
UniformType::fromGLValue(int type) {
	switch (type) {
	case GL_FLOAT:
		return FLOAT_1;
	case GL_FLOAT_VEC2:
		return FLOAT_2;
	case GL_FLOAT_VEC3:
		return FLOAT_3;
	case GL_FLOAT_VEC4:
		return FLOAT_4;
	case GL_INT:
	case GL_BOOL:
		return INT_1;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_2D_SHADOW:
		return SAMPLER;
	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		return INT_2;
	case GL_INT_VEC3:
	case GL_BOOL_VEC3:
		return INT_3;
	case GL_INT_VEC4:
	case GL_BOOL_VEC4:
		return INT_4;
	case GL_FLOAT_MAT2:
		return MAT_2;
	case GL_FLOAT_MAT3:
		return MAT_3;
	case GL_FLOAT_MAT4:
		return MAT_4;
	default:
		return UNSET;
	}
}

size_t
UniformType::getSize() const {
	size_t sz;
	switch (_val) {
	case UniformType::BOOL_1:
	case UniformType::UINT_1:
	case UniformType::INT_1:
	case UniformType::FLOAT_1:
	case UniformType::SAMPLER:
		sz = 4;
		break;
	case UniformType::INT_2:
	case UniformType::FLOAT_2:
		sz = 8;
		break;

	case UniformType::INT_3:
	case UniformType::FLOAT_3:
		sz = 12;
		break;
	case UniformType::INT_4:
	case UniformType::FLOAT_4:
		sz = 16;
		break;
	case UniformType::MAT_2:
		sz = 24;
		break;
	case UniformType::MAT_3:
		sz = sizeof(float)* 9;
		break;
	case UniformType::MAT_4:
		sz = sizeof(float)* 16;
		break;
	}
	return sz;
}
