/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <ge/Config.h>
#include <ge/Line3Base.h>
#include <vector>

namespace ge {
class Plane;
class Ray3 : public Line3Base {
protected:
    float _limit;
    /**
     * Ray vs triangle implementation.
     *
     * @param pointA
     * @param pointB
     * @param pointC
     * @param locationStore
     * @param doPlanar
     * @return true if this ray intersects a triangle formed by the given three points.
     * @throws NullPointerException
     *             if any of the points are null.
     */

    bool intersects( const glm::vec3& pointA,  const glm::vec3& pointB,
                     const glm::vec3& pointC,  glm::vec3& locationStore,  bool doPlanar) const;
public:
    /**
     * Constructs a new ray with an origin at (0,0,0) and a direction of (0,0,1).
     */
    Ray3();
    /**
     * Copy constructor.
     *
     * @param source
     *            the ray to copy from.
     */
//        Ray3(const Ray3& source);

    /**
     * Constructs a new ray using the supplied origin point and unit length direction vector
     *
     * @param origin
     * @param direction
     *            - unit length
     */
    Ray3( const glm::vec3& origin,  const glm::vec3& direction);

    ~Ray3();

    /**
     * <code>getLimit</code> returns the limit or the ray, aka the length.
     * If the limit is not infinity, then this ray is a line with length <code>
     * limit</code>.
     *
     * @return the limit or the ray, aka the length.
     */
    float getLimit() const {
        return _limit;
    }

    /**
     * <code>setLimit</code> sets the limit of the ray.
     * @param limit the limit of the ray.
     * @see Ray#getLimit()
     */
    void setLimit(float limit) {
        _limit = limit;
    }

    bool intersects(const std::vector<glm::vec3>&, glm::vec3& locationStore) const;

    /**
     * @param pointA
     * @param pointB
     * @param pointC
     * @param locationStore
     *            if not null, and this ray intersects, the point of intersection is calculated and stored in this
     *            glm::vec3
     * @return true if this ray intersects a triangle formed by the given three points.
     * @throws NullPointerException
     *             if any of the points are null.
     */
    bool intersectsTriangle( const glm::vec3& pointA,  const glm::vec3& pointB,
                             const glm::vec3& pointC,  glm::vec3& locationStore) const;

    /**
     * @param pointA
     * @param pointB
     * @param pointC
     * @param locationStore
     *            if not null, and this ray intersects, the point of intersection is calculated and stored in this
     *            glm::vec3 as (t, u, v) where t is the distance from the _origin to the point of intersection and (u, v)
     *            is the intersection point on the triangle plane.
     * @return true if this ray intersects a triangle formed by the given three points.
     * @throws NullPointerException
     *             if any of the points are null.
     */
    bool intersectsTrianglePlanar( const glm::vec3& pointA,  const glm::vec3& pointB,
                                   const glm::vec3& pointC,  glm::vec3& locationStore) const;

    /**
     * @param pointA
     * @param pointB
     * @param pointC
     * @param pointD
     * @param locationStore
     *            if not null, and this ray intersects, the point of intersection is calculated and stored in this
     *            glm::vec3
     * @return true if this ray intersects a triangle formed by the given three points. The points are assumed to be
     *         coplanar.
     * @throws NullPointerException
     *             if any of the points are null.
     */
    bool intersectsQuad( const glm::vec3& pointA,  const glm::vec3& pointB,
                         const glm::vec3& pointC,  const glm::vec3& pointD,  glm::vec3& locationStore) const;

    /**
     * @param pointA
     * @param pointB
     * @param pointC
     * @param pointD
     * @param locationStore
     *            if not null, and this ray intersects, the point of intersection is calculated and stored in this
     *            glm::vec3 as (t, u, v) where t is the distance from the _origin to the point of intersection and (u, v)
     *            is the intersection point on the triangle plane.
     * @return true if this ray intersects a quad formed by the given four points. The points are assumed to be
     *         coplanar.
     * @throws NullPointerException
     *             if any of the points are null.
     */
    bool intersectsQuadPlanar( const glm::vec3& pointA,  const glm::vec3& pointB,
                               const glm::vec3& pointC,  const glm::vec3& pointD,  glm::vec3& locationStore) const;

    /**
     * @param plane
     * @param locationStore
     *            if not null, and this ray intersects the plane, the world location of the point of intersection is
     *            stored in this vector.
     * @return true if the ray collides with the given Plane
     * @throws NullPointerException
     *             if the plane is null.
     */
    bool intersectsPlane( const Plane& plane,  glm::vec3& locationStore) const;

    /**
     * @param point
     * @param store
     *            if not null, the closest point is stored in this param
     * @return the squared distance from this ray to the given point.
     * @throws NullPointerException
     *             if the point is null.
     */
    float distanceSquared( const glm::vec3& point,  glm::vec3& store) const;

    /**
     * @param worldVertices
     *            an array of vectors describing a polygon
     * @return the distance from our origin to the primitive or POSITIVE_INFINITY if we do not intersect.
     */
    float getDistanceToPrimitive(const std::vector<glm::vec3>& worldVertices) const;
};
}

//std::ostream& operator<<(std::ostream&, const ge::Ray3&);
