#pragma once

#include <artemis/Config.h>
#include <artemis/EntitySystem.h>
#include <artemis/EntityManager.h>
#include <artemis/SystemBitManager.h>

#include <unordered_map>
#include <set>
#include <functional>
#include <memory>

namespace artemis {
	class Entity;
	class EntityManager;
	class EntityObserver;
	class EntitySystem;

	class World {
		EntityManager* _em;
		float _delta;

		std::set<std::shared_ptr<Entity> > _added;
		std::set<std::shared_ptr<Entity> > _changed;
		std::set<std::shared_ptr<Entity> > _deleted;
		std::set<std::shared_ptr<Entity> > _enable;
		std::set<std::shared_ptr<Entity> > _disable;

		std::unordered_map<std::string, EntitySystem*> _nameSystemsMap;
		std::vector<std::unique_ptr<EntitySystem> > _systems;

		std::unordered_map<std::string, Manager*> _nameManagersMap;
		std::vector<std::unique_ptr<Manager> > _managers;

	public:

		World();
		~World();

		template<typename T>
		T* setSystem(std::unique_ptr<T> up, bool passive = false) {
			//Check if system is known.
			for (size_t i = 0; i < _systems.size(); i++) {
				if (typeid(*up) == typeid(*_systems[i])){
					return static_cast<T*>(_systems[i].get());
				}
			}

			up->setPassive(passive);
			up->setWorld(this);

			_nameSystemsMap[typeid(*up).name()] = up.get();
			up->setSystemBit(SystemBitManager::getBitFor(typeid(*up)));
			_systems.push_back(std::move(up));
			return static_cast<T*>(_systems.back().get());
		}

		template<typename T>
		T* getSystem() {
			return (T*)(_nameSystemsMap[typeid(T).name()]);
		}

		template<typename T>
		T* setManager(std::unique_ptr<T> manager) {
			for (size_t i = 0; i < _managers.size(); i++) {
				if (typeid(*manager) == typeid(*_managers[i])){
					return static_cast<T*>(_managers[i].get());
				}
			}

			manager->setWorld(this);
			_nameManagersMap[typeid(*manager).name()] = manager.get();
			_managers.push_back(std::move(manager));
			return static_cast<T*>(_managers.back().get());
		}

		template<typename T>
		T* getManager() {
			return (T*)(_nameManagersMap[typeid(T).name()]);
		}

		EntityManager* getEntityManager() {
			return _em;
		}

		void initialize();

		float getDelta() const {
			return _delta;
		}
		void setDelta(float delta) {
			this->_delta = delta;
		}

		/**
		* Create and return a new or reused entity instance.
		*
		* Unlike the java implementation for memory management reasons
		* the entity is added to the world.
		*
		* @return entity
		*/
		std::shared_ptr<Entity> createEntity();

		/**
		* Get a entity having the specified id.
		*
		* @param entityId
		* @return entity
		*/
		std::shared_ptr<Entity> getEntity(int entityId) const;

		void process();

	private:

		friend class Entity;
		/**
		* Adds a entity to this world.
		*
		* @param e entity
		*/
		void addEntity(const std::shared_ptr<Entity>& e);

		/**
		* Ensure all systems are notified of changes to this entity.
		* If you're adding a component to an entity after it's been
		* added to the world, then you need to invoke this method.
		*
		* @param e entity
		*/
		void changedEntity(const std::shared_ptr<Entity>& e);

		/**
		* Delete the entity from the world.
		*
		* @param e entity
		*/
		void deleteEntity(const std::shared_ptr<Entity>& e);

		/**
		* (Re)enable the entity in the world, after it having being disabled.
		* Won't do anything unless it was already disabled.
		*/
		void enable(const std::shared_ptr<Entity>& e);

		/**
		* Disable the entity from being processed. Won't delete it, it will
		* continue to exist but won't get processed.
		*/
		void disable(const std::shared_ptr<Entity>& e);

		void check(std::set<std::shared_ptr<Entity> >& entities, std::function<void(EntityObserver* observer, const std::shared_ptr<Entity>& e)> func);
	};
}
