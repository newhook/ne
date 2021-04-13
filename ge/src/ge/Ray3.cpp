/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <ge/Config.h>
#include <ge/Ray3.h>
#include <ge/Plane.h>
#include <cmath>

using namespace ge;
using namespace glm;
using namespace std;

//ostream&
//operator<<(ostream& os, const ge::Ray3& ray) {
//    os << "Ray [origin: " << ray.getOrigin() << " direction: " << ray.getDirection() << "]";
//    return os;
//}

Ray3::Ray3() : Line3Base(glm::vec3(), glm::vec3(0.0f, 0.0f, 1.0f)), _limit(std::numeric_limits<float>::infinity()) {
}

Ray3::Ray3(const vec3& origin,  const vec3& direction) : Line3Base(origin,
	direction), _limit(std::numeric_limits<float>::infinity()) {
}

Ray3::~Ray3() {
}

bool
Ray3::intersects(const vector<vec3>& polygonVertices,  vec3& locationStore) const {
    if (polygonVertices.size() == 3) {
        // TRIANGLE
        return intersectsTriangle(polygonVertices[0], polygonVertices[1], polygonVertices[2], locationStore);
    } else if (polygonVertices.size() == 4) {
        // QUAD
        return intersectsQuad(polygonVertices[0], polygonVertices[1], polygonVertices[2], polygonVertices[3],
                              locationStore);
    }
    // TODO: Add support for line and point
    return false;
}

bool
Ray3::intersectsTriangle( const vec3& pointA,  const vec3& pointB,
                          const vec3& pointC,  vec3& locationStore) const {
    return intersects(pointA, pointB, pointC, locationStore, false);
}

bool
Ray3::intersectsTrianglePlanar( const vec3& pointA,  const vec3& pointB,
                                const vec3& pointC,  vec3& locationStore) const {
    return intersects(pointA, pointB, pointC, locationStore, true);
}

bool
Ray3::intersectsQuad( const vec3& pointA,  const vec3& pointB,
                      const vec3& pointC,  const vec3& pointD,  vec3& locationStore) const {
    return intersects(pointA, pointB, pointC, locationStore, false)
           || intersects(pointA, pointD, pointC, locationStore, false);
}

bool
Ray3::intersectsQuadPlanar( const vec3& pointA,  const vec3& pointB,
                            const vec3& pointC,  const vec3& pointD,  vec3& locationStore) const {
    return intersects(pointA, pointB, pointC, locationStore, true)
           || intersects(pointA, pointD, pointC, locationStore, true);
}

namespace {
	float EPSILON = 2.220446049250313E-16f;
}

bool
Ray3::intersects( const vec3& pointA,  const vec3& pointB, const vec3& pointC,  vec3& locationStore,
                  bool doPlanar) const {
    vec3 diff = _origin - pointA;
    vec3 edge1 = pointB - pointA;
    vec3 edge2 = pointC - pointA;
    vec3 norm = glm::cross(edge1, edge2);

    float dirDotNorm = glm::dot(_direction, norm);
    float sign;
    if (dirDotNorm > EPSILON) {
        sign = 1.0f;
    } else if (dirDotNorm < -EPSILON) {
        sign = -1.0f;
        dirDotNorm = -dirDotNorm;
    } else {
        // ray and triangle/quad are parallel
        return false;
    }

    vec3 diffxEdge2(glm::cross(diff, edge2));
    float dirDotDiffxEdge2 = sign * glm::dot(_direction, diffxEdge2);
    bool result = false;
    if (dirDotDiffxEdge2 >= 0.0f) {
        vec3 edge1xDiff(glm::cross(edge1, diff));
        float dirDotEdge1xDiff = sign * glm::dot(_direction, edge1xDiff);
        if (dirDotEdge1xDiff >= 0.0f) {
            if (dirDotDiffxEdge2 + dirDotEdge1xDiff <= dirDotNorm) {
                float diffDotNorm = -sign * glm::dot(diff, norm);
                if (diffDotNorm >= 0.0f) {
                    // ray intersects triangle
                    // if storage vector is null, just return true,
//                    if (locationStore == 0) {
//                        return true;
//                    }
                    // else fill in.
                    float inv = 1.0f / dirDotNorm;
                    float t = diffDotNorm * inv;
                    if (!doPlanar) {
                        locationStore = _origin + vec3(_direction.x * t, _direction.y * t, _direction.z * t);
                    } else {
                        // these weights can be used to determine
                        // interpolated values, such as texture coord.
                        // eg. texcoord s,t at intersection point:
                        // s = w0*s0 + w1*s1 + w2*s2;
                        // t = w0*t0 + w1*t1 + w2*t2;
                        float w1 = dirDotDiffxEdge2 * inv;
                        float w2 = dirDotEdge1xDiff * inv;
                        // float w0 = 1.0f - w1 - w2;
                        locationStore = vec3(t, w1, w2);
                    }
                    result = true;
                }
            }
        }
    }

    return result;
}

bool
Ray3::intersectsPlane(const Plane& plane, vec3& locationStore) const {
    vec3 normal = plane.getNormal();
    float denominator = glm::dot(normal, _direction);

    if (denominator > -EPSILON && denominator < EPSILON) {
        return false; // coplanar
    }

    float numerator = -glm::dot(normal, _origin) + plane.getConstant();
    float ratio = numerator / denominator;

    if (ratio < EPSILON) {
        return false; // intersects behind _origin
    }

//    if (locationStore != null) {
    locationStore = (_direction * ratio) + _origin;
//    }

    return true;
}

float
Ray3::distanceSquared( const vec3& point,  vec3& store) const {
    vec3 vectorA = point - _origin;
    float t0 = glm::dot(_direction, vectorA);
    if (t0 > 0) {
        // d = |P - (O + t*D)|
        vectorA = _direction * t0;
        vectorA += _origin;
    } else {
        // ray is closest to origin point
        vectorA = _origin;
    }

    // Save away the closest point if requested.
//    if (store != null) {
    store = vectorA;
//    }

    vectorA = point - vectorA;
	// lengthSquared.
	return vectorA.x * vectorA.x + vectorA.y * vectorA.y + vectorA.z * vectorA.z;
}

/**
 * @param worldVertices
 *            an array of vectors describing a polygon
 * @return the distance from our origin to the primitive or POSITIVE_INFINITY if we do not intersect.
 */
float
Ray3::getDistanceToPrimitive(const vector<vec3>& worldVertices) const {
    // Intersection test
    vec3 intersect;
    if (intersects(worldVertices, intersect)) {
        return glm::distance(getOrigin(), intersect);
    }
	return std::numeric_limits<float>::infinity();
}

