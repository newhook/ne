/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/LineGeom.h>
#include <ge/Lines.h>
#include <memory>

using namespace ge;
using namespace std;

LineGeom::LineGeom(const Lines& lines) : vbo(0) {
	offsets.resize(GeometryType::MAX + 1);
	size_t sz = 0;

	sz = lines.vertices.size() * sizeof(glm::vec3);
	stride = sizeof(glm::vec3);

	if (!lines.colors.empty()) {
		sz += lines.colors.size() * sizeof(glm::vec4);
		offsets[GeometryType::COLORS] = stride;
		stride += sizeof(glm::vec4);
	}

	if (sz == 0) {
		return;
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	unique_ptr<unsigned char> buf(new unsigned char[sz]);
	unsigned char* curr = buf.get() + offsets[GeometryType::VERTICES];
	for (size_t i = 0; i < lines.vertices.size(); ++i) {
		*((glm::vec3*)curr) = lines.vertices[i];
		curr += stride;
	}
	if (!lines.colors.empty()) {
		curr = buf.get() + offsets[GeometryType::COLORS];
		for (size_t i = 0; i < lines.colors.size(); ++i) {
			*((glm::vec4*)curr) = lines.colors[i];
			curr += stride;
		}
	}

	_size = lines.vertices.size();

	glBufferData(GL_ARRAY_BUFFER, sz, buf.get(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

LineGeom::~LineGeom() {
	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
}

void
LineGeom::render() {
	glDrawArrays(GL_LINES, 0, _size);
}