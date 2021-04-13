/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Config.h>
#include <ge/Bounds.h>
#include <ge/Transform.h>
#include <ge/Plane.h>

using namespace ge;

bool
ge::TestPlanesAABB(const std::vector<Plane>& planes, const Bounds& bound) {
	for (auto p = planes.begin(); p != planes.end(); ++p) {
		glm::vec3 normal = p->getNormal();
		const glm::vec3& extents = bound.getExtents();
		float radius = abs(extents.x * normal.x) + abs(extents.y * normal.y) + abs(extents.z * normal.z);

		float distance = p->distance(bound.getCenter());
		if (distance < -radius) {
			return false;
		}
	}
	return true;
}

Bounds::Bounds() {

}

Bounds::~Bounds() {

}

bool
Bounds::intersects(const Bounds& other) {
	if (fabs(_center.x - other._center.x) > (_extents.x + other._extents.x)) {
		return false;
	}
	if (fabs(_center.y - other._center.y) > (_extents.y + other._extents.y)) {
		return false;
	}
	if (fabs(_center.z - other._center.z) > (_extents.z + other._extents.z)) {
		return false;
	}

	// We have an overlap
	return true;
}

void
Bounds::compute(const glm::vec3& lower, glm::vec3& upper) {
	_center = (upper - lower) / 2.0f;
	_extents.x = (upper.x - lower.x) / 2;
	_extents.y = (upper.x - lower.x) / 2;
	_extents.z = (upper.x - lower.x) / 2;
}

void
Bounds::compute(const std::vector<glm::vec3>& vertices) {
	if (vertices.empty()) {
		return;
	}
	auto v = vertices.begin();
	glm::vec3 min = *v;
	++v;
	glm::vec3 max = min;

	for (; v != vertices.end(); ++v) {
		if (v->x < min.x) {
			min.x = v->x;
		}
		else if (v->x > max.x) {
			max.x = v->x;
		}

		if (v->y < min.y) {
			min.y = v->y;
		}
		else if (v->y > max.y) {
			max.y = v->y;
		}

		if (v->z < min.z) {
			min.z = v->z;
		}
		else if (v->z > max.z) {
			max.z = v->z;
		}
	}

	_center = glm::vec3(min.x + max.x, min.y + max.y, min.z + max.z) * 0.5f;
	_extents = glm::vec3(max.x - _center.x, max.y - _center.y, max.z - _center.z);
}

void
Bounds::transform(const Transform& transform) {
	_center = transform.transformVector(_center);

	// Find new extents using AABB's verts
	// Only 4 verts are needed because AABB is symmetric
	glm::vec3 verts[4] = {
		glm::vec3(_extents.x, _extents.y, _extents.z),
		glm::vec3(-_extents.x, _extents.y, _extents.z),
		glm::vec3(_extents.x, -_extents.y, _extents.z),
		glm::vec3(_extents.x, _extents.y, -_extents.z)
	};
	glm::vec3 newExtents;
	for (int i = 0; i < 4; ++i) {
		verts[i] = glm::abs(transform.getRotation() * (verts[i] * transform.getScale()));
		if (verts[i].x > newExtents.x) {
			newExtents.x = verts[i].x;
		}
		if (verts[i].y > newExtents.y) {
			newExtents.y = verts[i].y;
		}
		if (verts[i].z > newExtents.z) {
			newExtents.z = verts[i].z;
		}
	}
	_extents = newExtents;

#ifdef never
	_extents = glm::abs((_extents * transform.getScale()) * transform.getRotation());
#endif
	
}
