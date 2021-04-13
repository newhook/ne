#include <MouseDrive.h>
#include <CameraControl.h>

#include <SFML/Graphics.hpp>

using namespace glm;

MouseDrive::MouseDrive(ge::Camera* camera) : CameraBehavior(camera),
											_driveSpeed(10.0f)
{
}

MouseDrive::~MouseDrive()
{
}

void
MouseDrive::cameraChange(CameraBehavior* previous) {
}

CameraControl::BehaviorType
MouseDrive::getBehaviorType() {
	return CameraControl::BehaviorType::Drive;
}

void
MouseDrive::update(float deltaFrameTime)
{
	vec3 direction;
	ge::Transform transform = _camera->getTransform();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			//direction += -transform.getForward();
			direction += -glm::vec3(0, 1, 0);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			//direction += transform.getForward();
			direction += glm::vec3(0, 1, 0);
		}
	}
	else {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			//direction += -transform.getForward();
			direction += -glm::vec3(0, 0, 1);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			//direction += transform.getForward();
			direction += glm::vec3(0, 0, 1);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		//direction += -transform.getRight();
		direction += -glm::vec3(1, 0, 0);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		//direction += transform.getRight();
		direction += glm::vec3(1, 0, 0);
	}

	vec3 distanceMoved = deltaFrameTime * _driveSpeed * direction;
	transform.setTranslation(transform.getTranslation() + distanceMoved);
	_camera->setTransform(transform);
}