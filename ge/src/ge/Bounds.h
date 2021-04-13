/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Transform.h>
#include <vector>
namespace ge {
	// TestPlanes... belongs in a util.
class Bounds;
class Plane;
bool TestPlanesAABB(const std::vector<Plane>& planes, const Bounds& bound);

class Transform;
class Bounds {
	glm::vec3 _center;
	glm::vec3 _extents;

public:

	Bounds();
	virtual ~Bounds();
	
	bool intersects(const Bounds& other);
	void compute(const glm::vec3& lower, glm::vec3& upper);
	void compute(const std::vector<glm::vec3>& vertices);
	void transform(const Transform& transform);

	const glm::vec3& getCenter() const {
		return _center;
	}
	const glm::vec3& getExtents() const {
		return _extents;
	}
};

}