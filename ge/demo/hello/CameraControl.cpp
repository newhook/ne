#include <../demo/hello/CameraControl.h>
#include <../demo/hello/MouseOrbitZoom.h>
#include <../demo/hello/MouseDrive.h>

#include <SFML/Graphics.hpp>

using namespace ge;
using namespace std;

CameraControl::CameraControl(Camera* camera) : _activeBehavior(shared_ptr<MouseDrive>(new MouseDrive(camera))) {

	_behaviors.insert(make_pair(CameraControl::BehaviorType::Drive, _activeBehavior));
	_behaviors.insert(make_pair(CameraControl::BehaviorType::Orbit, shared_ptr<MouseOrbitZoom>(new MouseOrbitZoom(camera))));
	
	assert(_behaviors.size() == BehaviorType::Last);
}

CameraControl::~CameraControl(){
}

shared_ptr<CameraBehavior>
CameraControl::setBehavior(BehaviorType behaviorType) {
	if (behaviorType != _activeBehavior->getBehaviorType()) {
		shared_ptr<CameraBehavior> behavior = _behaviors[behaviorType];
		behavior->cameraChange(_activeBehavior.get());

		_activeBehavior = _behaviors[behaviorType];
		return  behavior;
	}

	return 0;
}

void
CameraControl::update(float deltaFrameTime) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))  {
		//for (int active = (int)_activeBehaviorType; active != )
		int nextIndex = static_cast<int>(_activeBehavior->getBehaviorType()) + 1;
		nextIndex = nextIndex % CameraControl::Last;

		setBehavior((CameraControl::BehaviorType)nextIndex);
		assert(_activeBehavior);
	}
	
	_activeBehavior->update(deltaFrameTime);	
}

std::shared_ptr<CameraBehavior> 
CameraControl::getBehaviorByType(BehaviorType behaviorType) {
	return _behaviors[behaviorType];
}