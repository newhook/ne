/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Bounds.h>
#include <ge/Mesh.h> // Geometry type.
#include <vector>

namespace ge {

class LineGeom;
class Lines {
	Bounds _bounds;

	std::shared_ptr<LineGeom> _geometry;

public:

	Lines();
	Lines(const Lines&);
	Lines& operator=(const Lines&);
	virtual ~Lines();

	std::shared_ptr<LineGeom> getGeometry();

	// Get bounds in local space.
	const Bounds& getBounds() const {
		return _bounds;
	}

	void refreshBounds();

	void clear();
	size_t getVertexCount() const;
	void setVertexCount(size_t vertexCount);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;
};

}