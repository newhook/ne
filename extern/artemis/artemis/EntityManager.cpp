#include <artemis/Config.h>

#include <artemis/EntityManager.h>
#include <artemis/EntitySystem.h>
#include <artemis/World.h>
#include <artemis/Component.h>
#include <artemis/Entity.h>

using namespace artemis;
using namespace std;

EntityManager::EntityManager() :
	_active(0),
	_added(0),
	_created(0),
	_deleted(0),
	_nextAvailableId(0)
{
}

EntityManager::~EntityManager() {

}

void EntityManager::added(const std::shared_ptr<Entity>& e) {
	_active++;
	_added++;
}

void EntityManager::changed(const std::shared_ptr<Entity>& e) {
	_disabled.erase(e->getId());
}

void EntityManager::deleted(const std::shared_ptr<Entity>& e) {
	_clean.insert(e->getId());
	_disabled.erase(e->getId());
	_availableIds.push_back(e->getId());

	_active--;
	_deleted++;
}

void EntityManager::enabled(const std::shared_ptr<Entity>& e) {
	_disabled.erase(e->getId());
}

void EntityManager::disabled(const std::shared_ptr<Entity>& e) {
	_disabled.insert(e->getId());
}

std::shared_ptr<Entity>
EntityManager::createEntityInstance() {
	int id;
	if (!_availableIds.empty()) {
		id = _availableIds.back();
		_availableIds.pop_back();
	}
	else {
		id = _nextAvailableId++;
	}
	cout << "create: " << id << endl;
	if (_entities.size() <= id) {
		_entities.resize(id + 1);
	}
	_entities[id] = make_shared<Entity>(_world, id);
	_created++;

	return _entities[id];
}

bool EntityManager::isActive(int entityId) {
	if (entityId >= _entities.size()) {
		return false;
	}
	return _entities[entityId] != nullptr;
}

bool EntityManager::isEnabled(int entityId) {
	return _disabled.count(entityId) == 0;
}

std::shared_ptr<Entity> EntityManager::getEntity(int entityId) const {
	if (entityId >= _entities.size()) {
		return nullptr;
	}
	return _entities[entityId];
}

void EntityManager::clean() {
	for (auto p = _clean.begin(); p != _clean.end(); ++p) {
		_entities[*p] = nullptr;
	}
	_clean.clear();
}