/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Transform.h>

#include <vector>
#include <map>
#include <memory>

namespace ge {

struct AnimationData {
	AnimationData() {

	}

	AnimationData(const glm::quat& r, const glm::vec3& t) {
		rotation = r;
		translation = t;
	}

	glm::quat rotation;
	glm::vec3 translation;
};

class AnimationClipState;

class AnimationChannel {
	std::vector<AnimationData> _data;
	std::vector<float> _times;

	void setCurrentSample(size_t sampleIndex, float progressPercent, AnimationData& applyTo) const;

public:

	AnimationChannel();
	AnimationChannel(const std::vector<AnimationData>& data);
	virtual ~AnimationChannel();

	std::vector<AnimationData>& getData() { return _data;  }
	std::vector<float>& getTimes() { return _times;  }

	void update(float clockTime, size_t jointIndex, AnimationClipState& clipState) const;
};

class AnimationClipState;
class AnimationClip {
	// Indexed by the joint index.
	std::vector<AnimationChannel> _channels;
	float _length;

public:

	AnimationClip(size_t channels);
	virtual ~AnimationClip();

	size_t size() const { return _channels.size(); }
	AnimationChannel& getChannel(size_t channel) { return _channels[channel]; }

	void setup();

	void update(float time, AnimationClipState& state);

	float length() const {
		return _length;
	}
};

class AnimationClipState {
	// Indexed by the joint index.
	std::vector<AnimationData> _jointState;
	std::vector<size_t> _frames;
	float _time;

public:

	AnimationClipState(size_t sz);
	~AnimationClipState();

	void update(float time) {
		_time = time;
	}

	size_t getFrame(size_t index, const std::vector<float>& times) {
		size_t max = times.size() - 1;
		while (_frames[index] < max && times[_frames[index] + 1] < _time) {
			++_frames[index];
		}
		return _frames[index];
	}
	
	size_t size() const {
		return _jointState.size();
	}

	AnimationData& get(size_t index) {
		return _jointState[index];
	}
};

struct Joint {
	/// A name, for display or debugging purposes.
	std::string name;

	/// The inverse transform of this Joint in its bind position.
	Transform inverseBindPose;

	short index;
	short parentIndex;
};

class Skeleton {
	const std::vector<Joint> _joints;

public:

	Skeleton(const std::vector<Joint>& joints);
	virtual ~Skeleton();

	const std::vector<Joint>& getJoints() const { return _joints; }
	size_t size() const { return _joints.size(); }
};

class SkeletonPose {
	std::shared_ptr<Skeleton> _skeleton;

	/** Local transforms for the joints of the associated skeleton. */
	std::vector<Transform> _localTransforms;

	/** Local transforms last frame. */
	std::vector<Transform> _lastLocalTransforms;

	/** Global transforms for the joints of the associated skeleton. Not saved to savable. */
	std::vector<Transform> _globalTransforms;

	/**
	* A palette of matrices used in skin deformation - basically the global transform X the inverse bind pose
	* transform. Not saved to savable.
	*/
	std::vector<glm::mat4> _matrixPalette;

public:

	SkeletonPose(const std::shared_ptr<Skeleton>& skeleton);
	virtual ~SkeletonPose();

	void updateTransforms();
	void setToBindPose();

	std::vector<Transform>& getLocalJointTransforms() { return _localTransforms;  }

	size_t size() const { return _skeleton->size(); }

	const std::vector<glm::mat4>& getBones() const { return _matrixPalette; }
};

class Animator {
	std::shared_ptr<SkeletonPose> _pose;
	std::map<std::string, std::shared_ptr<AnimationClip> > _clips;
	std::shared_ptr<AnimationClip> _curr;
	std::shared_ptr<AnimationClipState> _state;
	float _time;

public:

	Animator(const std::shared_ptr<SkeletonPose>& pose);
	~Animator();

	void addClip(const std::string& name, const std::shared_ptr<AnimationClip>& clip);

	void play(const std::string& name);

	void update(float time);

	const std::shared_ptr<SkeletonPose>& getPose() const { return _pose; }
};

}