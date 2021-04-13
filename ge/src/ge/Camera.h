/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <ge/Config.h>
#include <ge/Plane.h>
#include <ge/Transform.h>
#include <vector>

namespace ge {
class Ray3;
class Plane;
class BoundingVolume;
class Camera {
public:

	enum Type {
		ORTHO, FREE
	};
protected:

	Transform _transform;

	// Viewport x and y.
	int _vpX;
	int _vpY;

	// Window width and height.
	int _width;
	int _height;

	double _aspect;
	double _fov;
	double _nearClip;
	double _farClip;

	glm::mat4 _modelView;
	glm::mat4 _projection;
	glm::mat4 _modelViewProjection;
	glm::mat4 _modelViewProjectionInverse;

	Type _type;

	std::vector<Plane> _planes;

public:

	Camera();
	Camera(const Camera& source);

	Transform getTransform() {
		return _transform;
	}

	void setTransform(const Transform& transform);

	void setMode(Type cam_mode);
	Type getMode() const { return _type;  }
	void setFOV(double fov);
	void setViewport(int loc_x, int loc_y, int width, int height);
	void setViewport(const glm::ivec4& vp) {
		setViewport(vp.x, vp.y, vp.z, vp.w);
	}
	glm::ivec4 getViewport() const {
		return glm::ivec4(_vpX, _vpY, _width, _height);
	}
	void setClipping(double near_clip_distance, double far_clip_distance);

	const glm::mat4& getProjectionMatrix() const {
		return _projection;
	}

	const glm::mat4& getModelViewMatrix() const {
		return _modelView;
	}

	const glm::mat4& getModelViewProjectionMatrix() const {
		return _modelViewProjection;
	}

	const glm::mat4& getModelViewProjectionInverseMatrix() const {
		return _modelViewProjectionInverse;
	}

	const std::vector<Plane>& getPlanes() const {
		return _planes;
	}
	glm::vec3 screenPointToRay(int x, int y) const;

    void update();
};
}
