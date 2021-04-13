/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Mesh.h>
#include <ge/MeshGeom.h>
#include <memory>

using namespace ge;
using namespace std;

GLenum
PrimitiveType::getGLType() const {
	switch (_val) {
	case TRIANGLES:
		return GL_TRIANGLES;
	case TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	}
	return GL_TRIANGLES;
}

Mesh::Mesh() : _vertexCount(0), _mode(PrimitiveType::TRIANGLES) {
	instances.resize(4 + 1);
}

Mesh::Mesh(const Mesh& other) {
	_geometry = other._geometry;
	_mode = other._mode;
	triangles = other.triangles;
	vertices = other.vertices;
	normals = other.normals;
	colors = other.colors;
	uv = other.uv;
	uv2 = other.uv2;
	instances = other.instances;
	_vertexCount = other._vertexCount;
}

Mesh&
Mesh::operator=(const Mesh& other) {
	if (this != &other) {
		clear();

		_geometry = other._geometry;
		_mode = other._mode; 
		triangles = other.triangles;
		vertices = other.vertices;
		normals = other.normals;
		colors = other.colors;
		uv = other.uv;
		uv2 = other.uv2;
		instances = other.instances;
		_vertexCount = other._vertexCount;
	}
	return *this;
}

Mesh::~Mesh() {
	clear();
}

std::shared_ptr<MeshGeom>
Mesh::getGeometry() {
	if (!_geometry) {
		_geometry = make_unique<MeshGeom>(*this);
	}
	return _geometry;
}

void
Mesh::clear() {
	_geometry = nullptr;
	triangles.clear();
	vertices.clear();
	normals.clear();
	colors.clear();
	uv.clear();
	uv2.clear();
	instances.clear();
}

void
Mesh::refreshBounds() {
	_bounds.compute(vertices);
}

size_t
Mesh::getVertexCount() const {
	if (_vertexCount == 0) {
		return vertices.size();
	}
	return _vertexCount;
}
