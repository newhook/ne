/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Bounds.h>
#include <vector>
#include <memory>

namespace ge {

class PrimitiveType {
public:
	enum Type {
		UNSET = -1,
		TRIANGLES,
		TRIANGLE_STRIP
	};
private:
	Type _val;

public:

	PrimitiveType(int val) : _val(static_cast<Type>(val)) {
	}

	PrimitiveType(Type val) : _val(val) {
	}

	PrimitiveType() : _val(UNSET) {
	}

	operator Type() const { return _val; }

	GLenum getGLType() const;
};

class Bounds;
class MeshGeom;

struct BoneWeight {
	BoneWeight() {
		index0 = 0;
		index1 = 0;
		index2 = 0;
		index3 = 0;
		weight0 = 0.0f;
		weight1 = 0.0f;
		weight2 = 0.0f;
		weight3 = 0.0f;
	}

	short index0;
	short index1;
	short index2;
	short index3;
	float weight0;
	float weight1;
	float weight2;
	float weight3;
};

class Mesh {
	Bounds _bounds;
	size_t _vertexCount;
	PrimitiveType _mode;
	std::shared_ptr<MeshGeom> _geometry;

public:

	Mesh();
	Mesh(const Mesh&);
	Mesh& operator=(const Mesh&);
	virtual ~Mesh();

	std::shared_ptr<MeshGeom> getGeometry();

	PrimitiveType getPrimitiveType() const {
		return _mode;
	}
	void setPrimitiveType(PrimitiveType mode) {
		_mode = mode;
	}

	void clear();
	size_t getVertexCount() const;
	void setVertexCount(size_t vertexCount);

	// Get bounds in local space.
	const Bounds& getBounds() const {
		return _bounds;
	}

	void refreshBounds();

	struct SubMesh {
		SubMesh() : indexOffset(0), triangleCount(0) {}

		int indexOffset;
		int triangleCount;
	};
	std::vector<SubMesh> submeshes;

	std::vector<unsigned int> triangles;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> colors;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec2> uv2;

	std::vector<BoneWeight> boneWeights;

	// Indexed by geometry type.
	std::vector<unsigned int> instances;
};

}