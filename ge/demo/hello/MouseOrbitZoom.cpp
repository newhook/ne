#include <CameraControl.h>
#include <MouseOrbitZoom.h>

// TODO: Change the camera to be a shared_ptr.
MouseOrbitZoom::MouseOrbitZoom(ge::Camera* camera) : CameraBehavior(camera) {
	_camera = camera;
	_zoomRate = 40.0f;

	_desiredDistance = _minDistance = 10.0f;
	_maxDistance = 100.0f;

	_zooming = false;
	_rotating = false;

	_xSpeed = 2.0f;
	_ySpeed = 2.0f;
	_yMinLimit = -2;
	_yMaxLimit = 2;
}


MouseOrbitZoom::~MouseOrbitZoom(){
}

void 
MouseOrbitZoom::setTarget(const std::shared_ptr<ge::MeshR>& target){
	_target = target;
}

const std::shared_ptr<ge::MeshR>& 
MouseOrbitZoom::getTarget() const{
	return _target;
}


void 
MouseOrbitZoom::cameraChange(CameraBehavior* previous) {

}

CameraControl::BehaviorType
MouseOrbitZoom::getBehaviorType() {
	return CameraControl::BehaviorType::Orbit;
}

void
MouseOrbitZoom::update(float deltaFrameTime)
{
	if (!_target){
		return;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && 
		sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && 
		sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){

		sf::Vector2i position = sf::Mouse::getPosition();
		if (!_zooming){
			_lastMousePosition = position;
		}
		
		sf::Vector2i deltaMousePosition = position - _lastMousePosition;
		_lastMousePosition = position;
		
		_desiredDistance += deltaMousePosition.y * deltaFrameTime * _zoomRate * 0.125f * std::abs(_desiredDistance);

		_zooming = true;
	}else{
		_zooming = false;
	}

	_desiredDistance = glm::clamp(_desiredDistance, _minDistance, _maxDistance);


	glm::mat3 rotation = glm::mat3(_camera->getModelViewMatrix());

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){

		sf::Vector2i position = sf::Mouse::getPosition();
		if (!_rotating){
			_lastMousePosition = position;
		}

		sf::Vector2i deltaMousePosition = position - _lastMousePosition;
		_lastMousePosition = position;

		_xDeg += deltaMousePosition.x * _xSpeed * 0.02f;
		_yDeg -= deltaMousePosition.y * _ySpeed * 0.02f;


		//Clamp the vertical axis for the orbit
		_yDeg = ClampAngle(_yDeg, _yMinLimit, _yMaxLimit);
		// set camera rotation 

		glm::vec3 myEuler(_yDeg, _xDeg, 0);
		_desiredRotation = glm::quat(myEuler);
				
		rotation = glm::toMat3(_desiredRotation);
		rotation[0][0] *= -1;
		rotation[1][0] *= -1;
		rotation[2][0] *= -1;
					   
		rotation[0][2] *= -1;
		rotation[1][2] *= -1;
		rotation[2][2] *= -1;

		//_camera->setAxes(rotation);
		_rotating = true;
	}
	else{
		_rotating = false;
	}
	

	//glm::vec3 newPos = _target->getTransform().getTranslation() - (_camera->getDirection() * _desiredDistance);

	//_camera->setLocation(newPos);
}

float 
MouseOrbitZoom::ClampAngle(float angle, float min, float max)
{
	if (angle < -360)
		angle += 360;
	if (angle > 360)
		angle -= 360;
	return glm::clamp(angle, min, max);
}