#include <ge/Config.h>
#include <CameraBehavior.h>

#pragma once
class MouseDrive : public CameraBehavior
{
	float _driveSpeed;

public:

	MouseDrive(ge::Camera* camera);
	~MouseDrive();

	void cameraChange(CameraBehavior* previous);
	CameraControl::BehaviorType getBehaviorType();
	void update(float deltaFrameTime);
};

