/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Mesh.h>
#include <vector>

namespace ge {

class GeometryType {
public:
	enum Type {
		UNSET = -1,
		VERTICES = 0,
		NORMALS = 1,
		COLORS = 2,
		UV = 3,
		UV2 = 4,
		JOINTS = 5,
		WEIGHTS = 6,
		MAX = 6,
	};
private:
	Type _val;

public:

	GeometryType(int val) : _val(static_cast<Type>(val)) {
	}

	GeometryType(Type val) : _val(val) {
	}

	GeometryType() : _val(UNSET) {
	}

	operator Type() const { return _val; }
};

class MeshGeom {
	GLuint _ivbo;
	PrimitiveType _mode;
	std::vector<ge::Mesh::SubMesh> _submeshes;

	void deleteVBO();

	MeshGeom(const MeshGeom&);
	MeshGeom& operator=(const MeshGeom&);

public:

	MeshGeom(const Mesh& mesh);
	virtual ~MeshGeom();

	void render(size_t submesh);

	// Indexed by geometry type.
	std::vector<unsigned int> instances;

	// Internal use only.
	// Indexed by geometry type.
	std::vector<size_t> offsets;
	std::vector<GLuint> vbos;
	std::vector<size_t> strides;
};

}