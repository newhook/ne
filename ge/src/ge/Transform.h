/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

// Bullet.
class btTransform;

namespace ge {

glm::quat lookRotation(const glm::vec3& forward, const glm::vec3& up);

void swingTwist(const glm::quat& rotation, const glm::vec3& direction, glm::quat& swing, glm::quat& twist);

class Transform {
protected:

	glm::quat _rot;
	glm::vec3 _translation;
	glm::vec3 _scale;

public:

	Transform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rot) : _rot(rot),
		_translation(translation),
		_scale(scale) {
	}

	Transform(const glm::vec3 & translation, const glm::quat & rot) : _rot(rot),
		_translation(translation),
		_scale(glm::vec3(1.0f, 1.0f, 1.0f)) {
	}

	Transform(const glm::vec3 & translation) : _rot(glm::quat(1, 0, 0, 0)),
		_translation(translation),
		_scale(glm::vec3(1.0f, 1.0f, 1.0f)) {
	}

	explicit Transform(const glm::quat& rot) : _rot(rot),
		_translation(glm::vec3(0.0f, 0.0f, 0.0f)),
		_scale(glm::vec3(1.0f, 1.0f, 1.0f)) {
	}

	Transform() : _rot(glm::quat(1, 0, 0, 0)),
		_translation(glm::vec3(0.0f, 0.0f, 0.0f)),
		_scale(glm::vec3(1.0f, 1.0f, 1.0f)) {
	}

	Transform& operator=(const btTransform& transform);

	Transform& setRotation(const glm::quat& rot) {
		_rot = rot;
		return *this;
	}

	Transform& setTranslation(const glm::vec3& trans) {
		_translation = trans;
		return *this;
	}

	Transform& translate(const glm::vec3& trans) {
		_translation += trans;
		return *this;
	}

	const glm::vec3& getTranslation() const {
		return _translation;
	}

	Transform& setScale(const glm::vec3& scale) {
		_scale = scale;
		return *this;
	}

	Transform& setScale(float scale) {
		_scale = glm::vec3(scale, scale, scale);
		return *this;
	}

	const glm::vec3& getScale() const {
		return _scale;
	}

	const glm::quat& getRotation() const {
		return _rot;
	}

	Transform& combineWithParent(const Transform& parent) {
		_scale *= parent._scale;
		_rot = parent._rot * _rot;
		_translation = (parent._rot * (_translation * parent._scale)) + parent._translation;
		return *this;
	}

	Transform& invert();

	Transform& setTranslation(float x, float y, float z) {
		_translation = glm::vec3(x, y, z);
		return *this;
	}

	Transform& setScale(float x, float y, float z) {
		_scale = glm::vec3(x, y, z);
		return *this;
	}

	glm::vec3 transformVector(const glm::vec3& in) const {
		// multiply with scale first, then rotate, finally translate (cf.
		// Eberly)
		return _rot * (in * _scale) + _translation;
	}

	glm::vec3 transformInverseVector(const glm::vec3& in) const {
		return (glm::inverse(_rot) * (in - _translation)) / _scale;
	}

	void loadIdentity() {
		_translation = glm::vec3(0, 0, 0);
		_scale = glm::vec3(1, 1, 1);
		_rot = glm::quat(1, 0, 0, 0);
	}

	// The blue axis.
	glm::vec3 getForward() const;

	// The up axis.
	glm::vec3 getUp() const;

	// The red axis.
	glm::vec3 getRight() const;

	Transform& lookRotation(const glm::vec3& forward);
	Transform& lookRotation(const glm::vec3& forward, const glm::vec3& up);

	// Build a look at view matrix.
	Transform& lookAt(const Transform& t);
	Transform& lookAt(const Transform& t, const glm::vec3& up);

	glm::quat getTwist(const glm::vec3& direction);
	glm::quat getSwing(const glm::vec3& direction);
	void getSwingTwist(const glm::vec3& direction, glm::quat& swing, glm::quat& twist);

	void rotate(const glm::vec3& eulerAngles);

	// Get euler angles in degrees.
	glm::vec3 getEulerAngles() const;

	// Set the rotation from the euler angles (in degrees).
	Transform& setEulerAngles(const glm::vec3& angles);

	glm::mat4 getWorldMatrix() const;

	bool operator == (const Transform &other) const {
		if (_translation != other._translation) {
			return false;
		}

		if (_scale != other._scale) {
			return false;
		}

		if (_rot != other._rot) {
			return false;
		}

		return true;
	}

	bool operator != (const Transform &other) const {
		return !operator == (other);
	}

	bool isIdentity() const;
};

}