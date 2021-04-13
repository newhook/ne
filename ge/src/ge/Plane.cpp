/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <ge/Config.h>
#include <ge/Plane.h>

using namespace ge;
using namespace std;
using namespace glm;

Plane::Plane() : _normal(glm::vec3(0, 1, 0)),
    _constant(0) {
}

Plane::Plane(const vec3& normal, float constant) : _normal(normal),
    _constant(constant) {
}

void
Plane::setCartesian(float a, float b, float c, float d) {
	float sqr = a * a + b * b + c * c;
	_normal = glm::vec3(a, b, c) * glm::inversesqrt(sqr);
	_constant = -d * glm::inversesqrt(sqr);
}

void
Plane::setPlanePoints(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    _normal = glm::normalize(glm::cross(v2 - v1, vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z)));
    _constant = glm::dot(_normal, v1);
}

float
Plane::distance(const glm::vec3& point) const {
    return glm::dot(_normal, point) - _constant;
}

Halfspace
Plane::classify(const glm::vec3& point) const {
    float dis = distance(point);
    if (dis < 0) {
        return Halfspace::NEGATIVE;
    } else if (dis > 0) {
		return Halfspace::POSITIVE;
    } else {
		return Halfspace::ON_PLANE;
    }
}
