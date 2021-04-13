/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <ge/Config.h>
#include <ge/Camera.h>
#include <ge/Renderer.h>
#include <ge/Ray3.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace ge;
using namespace std;
using namespace glm;

Camera::Camera() {
	_planes.resize(6); // 6 frustum planes.
	_type = Camera::FREE;
	_fov = 45;
}

Camera::Camera(const Camera& source) {
	/// XXXX: TODO:
}

void
Camera::setTransform(const Transform& transform) {
	_transform = transform;
}

void
Camera::setMode(Type cam_mode) {
	_type = cam_mode;
	_transform.loadIdentity();
}
void
Camera::setFOV(double fov) {
	_fov = fov;
}
void
Camera::setViewport(int x, int y, int width, int height) {
	_vpX = x;
	_vpY = y;
	_width = width;
	_height = height;
	//need to use doubles division here, it will not work otherwise and it is possible to get a zero aspect ratio with integer rounding
	_aspect = double(width) / double(height);
}
void 
Camera::setClipping(double nearClip, double farClip) {
	_nearClip = nearClip;
	_farClip = farClip;
}

void
Camera::update() {
	if (_type == ORTHO) {
		// TODO: This isn't really correct.
		// It should be x - width and such and use the near/far clip.
		_projection = glm::ortho(0.0f, (float)_width, 0.0f, (float)_height, -1.0f, 1.0f);
	}
	else if (_type== FREE) {
		_projection = glm::perspective(_fov, _aspect, _nearClip, _farClip);

		// For right hand this is glm::translate(_transform.getTranslation());

		// TODO: Why -translation?
		glm::mat4 translation = glm::translate(-_transform.getTranslation());
		glm::mat4 rotation = glm::toMat4(_transform.getRotation());
		_modelView = rotation * translation;

		_modelViewProjection = _projection * _modelView;
		_modelViewProjectionInverse = glm::inverse(_modelViewProjection);
	}

	// Compute the fustrum planes.
	float a, b, c, d;

	// Left clipping plane
	a = _modelViewProjection[0][3] + _modelViewProjection[0][0];
	b = _modelViewProjection[1][3] + _modelViewProjection[1][0];
	c = _modelViewProjection[2][3] + _modelViewProjection[2][0];
	d = _modelViewProjection[3][3] + _modelViewProjection[3][0];

	_planes[0].setCartesian(a, b, c, d);

	// Right clipping plane
	a = _modelViewProjection[0][3] - _modelViewProjection[0][0];
	b = _modelViewProjection[1][3] - _modelViewProjection[1][0];
	c = _modelViewProjection[2][3] - _modelViewProjection[2][0];
	d = _modelViewProjection[3][3] - _modelViewProjection[3][0];
	_planes[1].setCartesian(a, b, c, d);

	// Top clipping plane
	a = _modelViewProjection[0][3] - _modelViewProjection[0][1];
	b = _modelViewProjection[1][3] - _modelViewProjection[1][1];
	c = _modelViewProjection[2][3] - _modelViewProjection[2][1];
	d = _modelViewProjection[3][3] - _modelViewProjection[3][1];
	_planes[2].setCartesian(a, b, c, d);

	// Bottom clipping plane
	a = _modelViewProjection[0][3] + _modelViewProjection[0][1];
	b = _modelViewProjection[1][3] + _modelViewProjection[1][1];
	c = _modelViewProjection[2][3] + _modelViewProjection[2][1];
	d = _modelViewProjection[3][3] + _modelViewProjection[3][1];
	_planes[3].setCartesian(a, b, c, d);

	// Near clipping plane
	a = _modelViewProjection[0][3] + _modelViewProjection[0][2];
	b = _modelViewProjection[1][3] + _modelViewProjection[1][2];
	c = _modelViewProjection[2][3] + _modelViewProjection[2][2];
	d = _modelViewProjection[3][3] + _modelViewProjection[3][2];
	_planes[4].setCartesian(a, b, c, d);

	// Far clipping plane
	a = _modelViewProjection[0][3] - _modelViewProjection[0][2];
	b = _modelViewProjection[1][3] - _modelViewProjection[1][2];
	c = _modelViewProjection[2][3] - _modelViewProjection[2][2];
	d = _modelViewProjection[3][3] - _modelViewProjection[3][2];
	_planes[5].setCartesian(a, b, c, d);
}

glm::vec3
Camera::screenPointToRay(int x, int y) const {
	// TODO: This ignores viewport.
	glm::vec4 rayEye = glm::inverse(getProjectionMatrix()) *
		glm::vec4((2.0f * x) / _width - 1.0f, 1.0f - (2.0f * y) / _height, -1.0f, 1.0f);
	return glm::normalize(glm::vec3(glm::inverse(getModelViewMatrix()) *
		glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f)));
}