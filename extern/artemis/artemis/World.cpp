#include <artemis/Config.h>
#include <artemis/World.h>
#include <artemis/Entity.h>
#include <artemis/EntityManager.h>
#include <artemis/EntitySystem.h>

using namespace artemis;
using namespace std;

World::World() :	
	_delta(0.0f)
{
	_em = setManager(make_unique<EntityManager>());
}

World::~World() {
	ComponentTypeManager::deleteComponentTypes();
	SystemBitManager::removeBitSets();
}

void World::initialize() {
	for (auto p = _managers.begin(); p != _managers.end(); ++p) {
		(*p)->initialize();
	}
	for (auto p = _systems.begin(); p != _systems.end(); ++p) {
		(*p)->initialize();
	}
}

std::shared_ptr<Entity> World::createEntity() {
	return _em->createEntityInstance();
}

std::shared_ptr<Entity> World::getEntity(int entityId) const {
	return _em->getEntity(entityId);
}

void World::addEntity(const std::shared_ptr<Entity>& e) {
	_added.insert(e);
}

void World::changedEntity(const std::shared_ptr<Entity>& e) {
	_changed.insert(e);
}

void World::deleteEntity(const std::shared_ptr<Entity>& e) {
	_deleted.insert(e);
}

void World::enable(const std::shared_ptr<Entity>& e) {
	_enable.insert(e);
}

void World::disable(const std::shared_ptr<Entity>& e) {
	_disable.insert(e);
}

void
World::check(std::set<std::shared_ptr<Entity> >& entities, std::function<void(EntityObserver* observer, const shared_ptr<Entity>& e)> func) {
	if (!entities.empty()) {
		for (auto p = entities.begin(); p != entities.end(); ++p) {
			for (auto q = _managers.begin(); q != _managers.end(); ++q) {
				func(q->get(), *p);
			}
			for (auto q = _systems.begin(); q != _systems.end(); ++q) {
				func(q->get(), *p);
			}
		}
		entities.clear();
	}
}

void
World::process() {
	check(_added, [](EntityObserver* observer, const std::shared_ptr<Entity>&e) { observer->added(e); });
	check(_changed, [](EntityObserver* observer, const std::shared_ptr<Entity>&e) { observer->changed(e); });
	check(_disable, [](EntityObserver* observer, const std::shared_ptr<Entity>&e) { observer->disabled(e); });
	check(_enable, [](EntityObserver* observer, const std::shared_ptr<Entity>&e) { observer->enabled(e); });
	check(_deleted, [](EntityObserver* observer, const std::shared_ptr<Entity>&e) { observer->deleted(e); });

	_em->clean();

	for (auto p = _systems.begin(); p != _systems.end(); ++p) {
		if (!(*p)->isPassive()) {
			(*p)->process();
		}
	}
}