/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Animation.h>

using namespace ge;
using namespace std;

namespace {
inline void nlerp(const glm::quat& s, const glm::quat& e, float invF, float f, glm::quat& r) {
	if ((s.x * e.x + s.y * e.y + s.z * e.z + s.w * e.w) < 0.0f) {
		f = -f;
	}
	r.w = s.w * invF + e.w * f;
	r.x = s.x * invF + e.x * f;
	r.y = s.y * invF + e.y * f,
		r.z = s.z * invF + e.z * f;
	float sq = r.x * r.x + r.y * r.y + r.z * r.z + r.w * r.w;
	if (sq > 0.0f) {
		sq = 1.0f / sqrt(sq);
		r.w *= sq;
		r.x *= sq;
		r.y *= sq;
		r.z *= sq;
	}
	else {
		r.w = 1.0f;
		r.x = r.y = r.z = 0.0f;
	}
}

inline void blend(const glm::vec3& translationFrom, const glm::vec3& translationTo, const glm::quat& rotationFrom,const glm::quat& rotationTo,
	float fromWeight, float toWeight, AnimationData& store) {
	float totalWeight = fromWeight + toWeight;
	if (totalWeight != 0.0f) {
		fromWeight /= totalWeight;
		toWeight /= totalWeight;
	}
	//if (blendMode == AnimationBlendMode::Blend) {
	store.translation = translationFrom * fromWeight + translationTo * toWeight;
	glm::quat r;
	nlerp(rotationFrom, rotationTo, fromWeight, toWeight, r);
	store.rotation = r;
	//}
	//else { // blendMode == AnimationBlendMode::Additive
	//	// XXX ALEX: Is this correct with masks?
	//	store.setRotation(rotationFrom * rotationTo);
	//	store.setTranslation(rotationFrom * translationTo + translationFrom);
	//}
}
}
AnimationChannel::AnimationChannel() {

}
AnimationChannel::AnimationChannel(const std::vector<AnimationData>& data) : _data(data) {

}

AnimationChannel::~AnimationChannel() {
}

void
AnimationChannel::setCurrentSample(size_t sampleIndex, float progressPercent, AnimationData& applyTo) const {

	// shortcut if we are fully on one sample or the next
	if (progressPercent == 0.0f) {
		applyTo.translation = _data[sampleIndex].translation;
		applyTo.rotation = _data[sampleIndex].rotation;
	}
	else if (progressPercent == 1.0f) {
		applyTo.translation = _data[sampleIndex+1].translation;
		applyTo.rotation = _data[sampleIndex+1].rotation;
	}
	else {
		blend( _data[sampleIndex].translation, _data[sampleIndex+1].translation,
			_data[sampleIndex].rotation, _data[sampleIndex + 1].rotation,
			1.0f - progressPercent, progressPercent, applyTo);
	}
	//LOG_T("%d: update %.3f to %s,%s,%s", _jointIndex, progressPercent, applyTo.getRotation(),
	//      applyTo.getScale(), applyTo.getTranslation());
}

void
AnimationChannel::update(float clockTime, size_t jointIndex, AnimationClipState& clipState) const {
	// figure out what frames we are between and by how much
	size_t lastFrame = _times.size() - 1;
	if (clockTime < 0 || _times.size() == 1) {
		setCurrentSample(0, 0.0f, clipState.get(jointIndex));
	}
	else if (clockTime >= _times[lastFrame]) {
		setCurrentSample(lastFrame, 0.0f, clipState.get(jointIndex));
	}
	else {
		size_t startFrame = clipState.getFrame(jointIndex, _times);
		float progressPercent = (clockTime - _times[startFrame]) / (_times[startFrame + 1] - _times[startFrame]);
		setCurrentSample(startFrame, progressPercent, clipState.get(jointIndex));
	}
}

AnimationClip::AnimationClip(size_t channels) {
	_channels.resize(channels);

}
AnimationClip::~AnimationClip() {

}

void AnimationClip::update(float time, AnimationClipState& state) {
	size_t joint = 0;
	// Go through each channel and update clipState
	for (auto p : _channels) {
		p.update(time, joint, state);
	}
}
void AnimationClip::setup() {
	_length = 0;
	for (auto p : _channels) {
		if (!p.getTimes().empty()) {
			_length = glm::max(p.getTimes()[p.getTimes().size()], _length);
		}
	}
}

AnimationClipState::AnimationClipState(size_t sz) {
	_jointState.resize(sz);
	_frames.resize(sz);
}

AnimationClipState::~AnimationClipState() {
}

Skeleton::Skeleton(const std::vector<Joint>& joints) : _joints(joints) {
}

Skeleton::~Skeleton() {
}

SkeletonPose::SkeletonPose(const std::shared_ptr<Skeleton>& skeleton) : _skeleton(skeleton) {
	size_t jointCount = _skeleton->getJoints().size();

	_lastLocalTransforms.resize(jointCount);
	_localTransforms.resize(jointCount);
	_globalTransforms.resize(jointCount);
	_matrixPalette.resize(jointCount);
}

SkeletonPose::~SkeletonPose() {

}

void
SkeletonPose::updateTransforms() {
	// we go in update array order, which ensures parent global transforms are updated before child.
	// final int[] orders = _skeleton.getJointOrders();
	const std::vector<Joint>& joints = _skeleton->getJoints();

	size_t max = joints.size();
	bool dirty = false;

	for (size_t i = 0; i < max; ++i) {
		if (!dirty && _localTransforms[i] == _lastLocalTransforms[i]) {
			continue;
		}
		_lastLocalTransforms[i] = _localTransforms[i];
		dirty = true;

		_globalTransforms[i] = _localTransforms[i];

		// find our parent
		const short parentIndex = joints[i].parentIndex;
		if (parentIndex != -1) {
			// we have a parent, so take us from local->parent->model space by multiplying by parent's local->model
			// space transform.
			_globalTransforms[i].combineWithParent(_globalTransforms[parentIndex]);
		}

		// at this point we have a local->model space transform for this joint, for skinning we multiply this by the
		// joint's inverse bind pose (joint->model space, inverted). This gives us a transform that can take a
		// vertex from bind pose (model space) to current pose (model space).
		Transform tmp = joints[i].inverseBindPose;
		tmp.combineWithParent(_globalTransforms[i]);

		_matrixPalette[i] = tmp.getWorldMatrix();
	}
}

void
SkeletonPose::setToBindPose() {
	const std::vector<Joint>& joints = _skeleton->getJoints();
	const size_t max = _localTransforms.size();
	for (size_t i = 0; i < max; ++i) {
		_localTransforms[i] = joints[i].inverseBindPose;
		_localTransforms[i].invert();

		// At this point we are in model space, so we need to remove our parent's transform (if we have one.)
		const short parentIndex = joints[i].parentIndex;
		if (parentIndex != -1) {
			// We remove the parent's transform simply by multiplying by its inverse bind pose. Done! :)
			_localTransforms[i].combineWithParent(joints[parentIndex].inverseBindPose);
		}

		_lastLocalTransforms[i] = _localTransforms[i];

		_globalTransforms[i] = _localTransforms[i];
		if (parentIndex != -1) {
			// we have a parent, so take us from local->parent->model space by multiplying by parent's local->model
			// space transform.
			_globalTransforms[i].combineWithParent(_globalTransforms[parentIndex]);
		}
		Transform tmp = joints[i].inverseBindPose;
		tmp.combineWithParent(_globalTransforms[i]);
		_matrixPalette[i] = tmp.getWorldMatrix();
	}
}


Animator::Animator(const std::shared_ptr<SkeletonPose>& pose) : _pose(pose) {
	_pose->setToBindPose();
	_state = make_shared<AnimationClipState>(_pose->size());
}

Animator::~Animator() {

}

void Animator::addClip(const std::string& name, const std::shared_ptr<AnimationClip>& clip) {
	_clips.insert(make_pair(name, clip));
}

void Animator::play(const std::string& name) {
	auto p = _clips.find(name);
	if (p == _clips.end()) {
		_curr = nullptr;
		return;
	}

	_curr = p->second;
	_time = 0.0f;
}

void Animator::update(float time) {
	if (!_curr) {
		return;
	}
	_time += time;
	_time = glm::mod(_time, _curr->length());
	_curr->update(_time, *_state);

	for (size_t i = 0; i < _state->size(); ++i) {
		const AnimationData& jd = _state->get(i);
		if (_pose->getLocalJointTransforms().size() > i) {
			_pose->getLocalJointTransforms()[i].setTranslation(jd.translation);
			_pose->getLocalJointTransforms()[i].setRotation(jd.rotation);
		}
	}
	_pose->updateTransforms();
}
