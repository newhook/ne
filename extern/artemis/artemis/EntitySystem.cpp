#include <artemis/EntitySystem.h>
#include <artemis/World.h>
#include <artemis/Entity.h>

#include <algorithm>

using namespace artemis;
using namespace std;

EntitySystem::EntitySystem(const Aspect& aspect) : _aspect(aspect),
	_allSet(aspect.getAllSet()),
	_exclusionSet(aspect.getExclusionSet()),
	_oneSet(aspect.getOneSet()),
	_passive(false)
{
	this->world = nullptr;
}

EntitySystem::~EntitySystem() {
	world = nullptr;
}

void EntitySystem::check(const std::shared_ptr<Entity>& e) {
	bool contains = (_systemBit & e->getSystemBits()) == _systemBit;

	bool interested = true; // possibly interested, let's try to prove it wrong.

	const bitset<BITSIZE>& componentBits = e->getComponentBits();

	// Check if the entity possesses ALL of the components defined in the aspect.
	if (!_allSet.none()) {
		for (size_t i = 0; i < _allSet.size(); ++i) {
			if (_allSet.test(i) && !componentBits.test(i)) {
				interested = false;
				break;
			}
		}
	}

	// Check if the entity possesses ANY of the exclusion components, if it does then the system is not interested.
	if (!_exclusionSet.none() && interested) {
		interested = !(_exclusionSet & componentBits).any();
	}

	// Check if the entity possesses ANY of the components in the oneSet. If so, the system is interested.
	if (!_oneSet.none()) {
		interested = (_oneSet & componentBits).any();
	}

	if (interested && !contains) {
		insertToSystem(e);
	}
	else if (!interested && contains) {
		removeFromSystem(e);
	}
}

void EntitySystem::begin() {
}

void EntitySystem::end() {
}

void EntitySystem::removed(const std::shared_ptr<Entity>& e) {
}

void EntitySystem::inserted(const std::shared_ptr<Entity>& e) {
}

void EntitySystem::process() {
	if(checkProcessing()) {
		begin();
		processEntities(_actives);
		end();
	}
}

void EntitySystem::removeFromSystem(const std::shared_ptr<Entity>& e) {
	auto p = find(_actives.begin(), _actives.end(), e);
	if (p != _actives.end()) {
		std::swap(*p, _actives.back());
		_actives.pop_back();
	}
	e->getSystemBits() &= ~_systemBit;
	removed(e);
}

void EntitySystem::insertToSystem(const std::shared_ptr<Entity>& e) {
	assert(find(_actives.begin(), _actives.end(), e) == _actives.end());

	_actives.push_back(e);
	e->getSystemBits() |= _systemBit;
	inserted(e);
}

void EntitySystem::added(const std::shared_ptr<Entity>& e) {
	check(e);
}

void EntitySystem::changed(const std::shared_ptr<Entity>& e) {
	check(e);
}

void EntitySystem::deleted(const std::shared_ptr<Entity>& e) {
	if((e->getSystemBits() & _systemBit).any()) {
		removeFromSystem(e);
	}
}

void EntitySystem::enabled(const std::shared_ptr<Entity>& e) {
	check(e);
}

void EntitySystem::disabled(const std::shared_ptr<Entity>& e) {
	if ((e->getSystemBits() & _systemBit).any()) {
		removeFromSystem(e);
	}
}