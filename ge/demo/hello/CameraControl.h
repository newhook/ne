#pragma once
//#include <../demo/hello/CameraBehavior.h>
#include <ge/Camera.h>

#include <memory>
#include <map>

class CameraBehavior;

class CameraControl
{
public:
	enum BehaviorType {
		Orbit,
		Drive,
		Last
	};
private:
	std::map<CameraControl::BehaviorType, std::shared_ptr<CameraBehavior>> _behaviors;	
	std::shared_ptr<CameraBehavior> _activeBehavior;

public:

	CameraControl(ge::Camera* camera);
	~CameraControl();

	std::shared_ptr<CameraBehavior> setBehavior(BehaviorType behaviorType);
	void update(float deltaFrameTime);

	std::shared_ptr<CameraBehavior> getBehaviorByType(BehaviorType behaviorType);
};

