/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/MeshGeom.h> // Geometry type.
#include <vector>

namespace ge {

class Lines;
class LineGeom {

	size_t _size;

	LineGeom(const LineGeom&);
	LineGeom& operator=(const LineGeom&);

public:

	LineGeom(const Lines&);
	virtual ~LineGeom();

	void render();

	GLuint vbo;
	size_t stride;
	std::vector<size_t> offsets;
};

}