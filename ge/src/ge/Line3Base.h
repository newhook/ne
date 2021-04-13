/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <ge/Config.h>

namespace ge {
class Line3Base {
protected:
    glm::vec3 _origin;
    glm::vec3 _direction;

public:
    Line3Base () {
        _origin = glm::vec3();
		_direction = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    Line3Base( const glm::vec3 & origin,  const glm::vec3 & direction) {
        _origin = origin;
        _direction = direction;
    }

    /**
     * @return this line's origin point as a readable vector
     */
    const glm::vec3& getOrigin() const {
        return _origin;
    }

    /**
     * @return this line's direction as a readable vector
     */
    const glm::vec3& getDirection() const {
        return _direction;
    }

    /**
     * Sets the line's origin point to the values of the given vector.
     *
     * @param origin
     * @throws NullPointerException
     *             if normal is null.
     */
    void setOrigin( const glm::vec3& origin) {
        _origin = origin;
    }

    /**
     * Sets the line's direction to the values of the given vector.
     *
     * @param direction
     * @throws NullPointerException
     *             if direction is null.
     */
    void setDirection( const glm::vec3& direction) {
        _direction = direction;
    }

//        /**
//         * @return returns a unique code for this line3base object based on its values. If two line3base objects are
//         *         numerically equal, they will return the same hash code value.
//         */
//        @Override
//        int hashCode() {
//            int result = 17;
//
//            result += 31 * result + _origin.hashCode();
//            result += 31 * result + _direction.hashCode();
//
//            return result;
//        }
};
}
