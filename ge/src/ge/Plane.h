/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <ge/Config.h>

#include <ostream>

namespace ge { class Plane; }

namespace ge {
class Halfspace {
public:

	enum Type {
		UNSET = -1,
		NEGATIVE,
		POSITIVE,
		ON_PLANE
	};
	
private:

	Type _val;

public:

	Halfspace() : _val(UNSET) {
	}

	Halfspace(Type val) : _val(val) {
	}

	operator Type() const { return _val; }
};

class Plane {
    glm::vec3 _normal;
    float _constant;

public:

    Plane();
    Plane(const glm::vec3 &, float);

	void setCartesian(float a, float b, float c, float d);
    void setPlanePoints(const glm::vec3&, const glm::vec3&, const glm::vec3&);

    float getConstant() const {
        return _constant;
    }

    const glm::vec3& getNormal() const {
        return _normal;
    }

    float distance(const glm::vec3& point) const;
	Halfspace classify(const glm::vec3& point) const;
};
}
