#include <artemis/DelayedEntitySystem.h>
#include <artemis/World.h>

using namespace artemis;

DelayedEntitySystem::DelayedEntitySystem(const Aspect& aspect) : EntitySystem(aspect) {
	_acc = 0;
	_delay = 0;
	_running = false;
	_autoRepeat = false;
}

bool DelayedEntitySystem::checkProcessing() {
	if(_running) {
		_acc += world->getDelta();

		if(_acc >= _delay) {
			return true;
		}
	}

	return false;
}

float DelayedEntitySystem::getInitialTimeDelay() const {
	return _delay;
}

float DelayedEntitySystem::getRemainingTime() const {
	if(_running) {
		return _delay - _acc;
	}

	return 0;
}

bool DelayedEntitySystem::isRunning() const {
	return _running;
}

void DelayedEntitySystem::setAutoRepeat(bool repeat) {
	_autoRepeat = repeat;
}

void DelayedEntitySystem::processEntities(const std::vector<std::shared_ptr<Entity>>& bag) {
	processEntities(bag, _acc);

	if(_autoRepeat) {
		start(_delay);
	} else {
		stop();
	}
}

void DelayedEntitySystem::start(float delay) {
	_delay = delay;
	_acc = 0.0f;
	_running = true;
}

void DelayedEntitySystem::stop() {
	_acc = 0;
	_running = false;
}
