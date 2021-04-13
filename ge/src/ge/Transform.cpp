/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Transform.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <LinearMath/btTransform.h>

#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/projection.hpp>

using namespace ge;

glm::quat
ge::lookRotation(const glm::vec3& forward, const glm::vec3& up) {
	glm::vec3 f = glm::orthonormalize(forward, up);
	glm::vec3 right = glm::cross(up, f);
	glm::mat3 m = { right, up, f };
	return glm::toQuat(m);
}

// axis - the normalized axis for which to get the swing and twist rotation
// swing - will receive the swing rotation : the rotation around an axis perpendicular to the specified axis
// twist - will receive the twist rotation : the rotation around the specified axis
void
ge::swingTwist(const glm::quat& rotation, const glm::vec3& direction, glm::quat& swing, glm::quat& twist) {
	glm::vec3 ra(rotation.x, rotation.y, rotation.z); // rotation axis
	glm::vec3 p = glm::proj(ra, direction); // return projection v1 on to v2  (parallel component)
	twist = glm::normalize(glm::quat(rotation.w, p.x, p.y, p.z));
	swing = rotation * glm::conjugate(twist);
}


Transform&
Transform::operator=(const btTransform& trans) {
	setTranslation(glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	setRotation(glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()));
	// No scale.
	return *this;
}

Transform&
Transform::invert() {
	_rot = glm::inverse(_rot);
	_translation = -(_rot * _translation * _scale);
	return *this;
}

bool
Transform::isIdentity() const {
	if (_translation != glm::vec3(0, 0, 0)) {
		return false;
	}

	if (_scale != glm::vec3(1, 1, 1)) {
		return false;
	}

	if (_rot != glm::quat(1, 0, 0, 0)) {
		return false;
	}

	return true;
}

glm::vec3
Transform::getEulerAngles() const {
	return glm::eulerAngles(_rot);
}

Transform&
Transform::setEulerAngles(const glm::vec3& angles) {
	_rot = glm::quat(angles);
	return *this;
}

glm::quat
Transform::getTwist(const glm::vec3& direction) {
	glm::quat swing, twist;
	ge::swingTwist(_rot, direction, swing, twist);
	return twist;
}

glm::quat
Transform::getSwing(const glm::vec3& direction) {
	glm::quat swing, twist;
	ge::swingTwist(_rot, direction, swing, twist);
	return swing;
}
void 
Transform::getSwingTwist(const glm::vec3& direction, glm::quat& swing, glm::quat& twist) {
	ge::swingTwist(_rot, direction, swing, twist);
}

void
Transform::rotate(const glm::vec3& eulerAngles) {
	// TODO:
}

// The blue axis.
glm::vec3
Transform::getForward() const {
	return _rot * glm::vec3(0, 0, 1);
}

// The up axis.
glm::vec3
Transform::getUp() const {
	return _rot * glm::vec3(0, 1, 0);;
}

// The red axis.
glm::vec3
Transform::getRight() const {
	return _rot * glm::vec3(1, 0, 0);;
}

Transform&
Transform::lookRotation(const glm::vec3& forward) {
	return lookRotation(forward, glm::vec3(0, 1, 0));
}

Transform& 
Transform::lookRotation(const glm::vec3& forward, const glm::vec3& up) {
	_rot = ge::lookRotation(forward, up);
	return *this;
}

Transform&
Transform::lookAt(const Transform& t) {
	return lookAt(t, glm::vec3(0, 1, 0));
}

Transform&
Transform::lookAt(const Transform& t, const glm::vec3& up) {
	glm::mat4 lookMatrix = glm::lookAt(_translation, t.getTranslation(), up);
	_rot = glm::quat(lookMatrix);

	// TODO: There is something wrong here as the translation component of the lookMatrix is thrown out.
	//_translation = glm::vec3(lookMatrix[3]);

	return *this;
}

glm::mat4
Transform::getWorldMatrix() const {
	// TODO: Perhaps faster?

	//float* arr = (float*)&mat;
	//glm::quat q = glm::normalize(_rot);
	//arr[0] = (1 - 2 * q.y * q.y - 2 * q.z * q.z) * _scale.x;
	//arr[1] = (2 * q.x * q.y + 2 * q.w * q.z) * _scale.x;
	//arr[2] = (2 * q.x * q.z - 2 * q.w * q.y) * _scale.x;
	//arr[3] = 0.0f;
	//arr[4] = (2 * q.x * q.y - 2 * q.w * q.z) * _scale.y;
	//arr[5] = (1 - 2 * q.x * q.x - 2 * q.z * q.z) * _scale.y;
	//arr[6] = (2 * q.y * q.z + 2 * q.w * q.x) * _scale.y;
	//arr[7] = 0.0f;
	//arr[8] = (2 * q.x * q.z + 2 * q.w * q.y) * _scale.z;
	//arr[9] = (2 * q.y * q.z - 2 * q.w * q.x) * _scale.z;
	//arr[10] = (1 - 2 * q.x * q.x - 2 * q.y * q.y) * _scale.z;
	//arr[11] = 0.0f;
	//arr[12] = _translation.x;
	//arr[13] = _translation.y;
	//arr[14] = _translation.z;
	//arr[15] = 1;

	glm::mat4 scale = glm::scale(_scale);
	glm::mat4 translation = glm::translate(_translation);
	glm::mat4 rotation = glm::toMat4(_rot);

	return translation * rotation * scale;
}