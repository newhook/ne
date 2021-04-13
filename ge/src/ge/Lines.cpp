/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Lines.h>
#include <ge/LineGeom.h>
#include <ge/MeshGeom.h>
#include <memory>

using namespace ge;
using namespace std;

Lines::Lines() {
}

Lines::Lines(const Lines& other) {
	_geometry = other._geometry;
	vertices = other.vertices;
	colors = other.colors;
}

Lines&
Lines::operator=(const Lines& other) {
	if (this != &other) {
		clear();

		_geometry = other._geometry;
		vertices = other.vertices;
		colors = other.colors;
	}
	return *this;
}

Lines::~Lines() {
	clear();
}

std::shared_ptr<LineGeom>
Lines::getGeometry() {
	if (!_geometry) {
		_geometry = make_unique<LineGeom>(*this);
	}
	return _geometry;
}

void
Lines::clear() {
	_geometry = nullptr;
	vertices.clear();
	colors.clear();
}

void
Lines::refreshBounds() {
	_bounds.compute(vertices);
}

size_t
Lines::getVertexCount() const {
	return vertices.size();
}
