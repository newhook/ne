#pragma once
#include <CameraControl.h>
#include <ge/Config.h>

class CameraBehavior
{
protected:
	ge::Camera* _camera;

public:

	CameraBehavior(ge::Camera* camera);

	virtual void cameraChange(CameraBehavior* previous) = 0;
	virtual CameraControl::BehaviorType getBehaviorType() = 0;
	virtual void update(float deltaFrameTime) = 0;
};

