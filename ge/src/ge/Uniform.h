/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

namespace ge {

// The uniform value. To interpret the contents you must know
// the uniform type. 
union UniformValue {
	// For FLOAT_1/INT_1.
	int intValue0;
	float floatValue0;
	// For everything else (vec2, vec3 and so forth).
	int* intValue;
	float* floatValue;
	char* raw;
};

class UniformType {
public:

	enum Type {
		UNSET = -1,
		BOOL_1,

		UINT_1,

		INT_1,
		INT_2,
		INT_3,
		INT_4,
		FLOAT_1,
		FLOAT_2,
		FLOAT_3,
		FLOAT_4,
		MAT_2,
		MAT_3,
		MAT_4,
		SAMPLER
	};
private:
	Type _val;

public:

	UniformType(int val) : _val(static_cast<Type>(val)) {
	}

	UniformType(Type val) : _val(val) {
	}

	UniformType() : _val(UNSET) {
	}

	operator Type() const { return _val; }

	static UniformType fromGLValue(int type);

	size_t getSize() const;
	int getGLValue() const;
};

}