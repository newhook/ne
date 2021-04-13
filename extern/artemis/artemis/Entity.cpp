#include <artemis/Config.h>
#include <artemis/Entity.h>
#include <artemis/ComponentTypeManager.h>
#include <artemis/Component.h>
#include <artemis/World.h>

#include <sstream>

using namespace std;
using namespace artemis;

namespace {
	long nextUniqueId = 0;
}

Entity::Entity(World* world, int id) : _id(id), _world(world) {
	_uniqueId = nextUniqueId++;
}

Entity::~Entity() {
}

Entity& Entity::addComponent(std::unique_ptr<Component> c) {
	ComponentType type = ComponentTypeManager::getTypeFor(typeid(*c));
	if (_components.size() <= type.getId()) {
		_components.resize(type.getId() + 1);
	}
	_componentBits |= type.getBit();
	_components[type.getId()] = std::move(c);
	return *this;
}

Entity& Entity::removeComponent(const ComponentType& type) {
	_components[type.getId()] = nullptr;
	_componentBits &= ~type.getBit();

	return *this;
}

void Entity::reset() {
	_componentBits = 0;
	_systemBits = 0;
}

std::string Entity::toString() {
	std::ostringstream oss;
	oss << "Entity[" << _id << "]\n";
	return  oss.str();
}

bool Entity::isActive()  const {
	return _world->getEntityManager()->isActive(_id);
}

bool Entity::isEnabled() const  {
	return _world->getEntityManager()->isEnabled(_id);
}

void Entity::addToWorld() {
	_world->addEntity(shared_from_this());
}

void Entity::changedInWorld() {
	_world->changedEntity(shared_from_this());
}

void Entity::deleteFromWorld() {
	_world->deleteEntity(shared_from_this());
}

void Entity::enable() {
	_world->enable(shared_from_this());
}

void Entity::disable() {
	_world->disable(shared_from_this());
}