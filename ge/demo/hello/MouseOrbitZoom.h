#pragma once
#include <CameraBehavior.h>
#include <ge/Config.h>
#include <ge/Camera.h>
#include <ge/MeshR.h>

#include <SFML/Graphics.hpp>

#include <memory>

class MouseOrbitZoom : public CameraBehavior
{
	std::shared_ptr<ge::MeshR> _target;

	float _minDistance; 
	float _maxDistance;
	float _desiredDistance;
	float _zoomRate;

	bool _zooming;
	bool _rotating;
	sf::Vector2i _lastMousePosition;

	//Rotation Variables
	float _xDeg;
	float _yDeg;
	float _xSpeed;
	float _ySpeed;
	int _yMinLimit;
	int _yMaxLimit;
	glm::quat _desiredRotation;

public:

	MouseOrbitZoom(ge::Camera* camera);
	~MouseOrbitZoom();

	void setTarget(const std::shared_ptr<ge::MeshR>& target);
	const std::shared_ptr<ge::MeshR>& getTarget() const;

	void cameraChange(CameraBehavior* previous);
	CameraControl::BehaviorType	getBehaviorType();
	void update(float deltaFrameTime);


	static float MouseOrbitZoom::ClampAngle(float angle, float min, float max);
};

