#pragma once

#include <artemis/Config.h>

#include <memory>

namespace artemis {
	class World;
	class Entity;

	class EntityObserver {
	public:
		virtual ~EntityObserver() {

		}

		virtual void added(const std::shared_ptr<Entity>& e) = 0;
		virtual void changed(const std::shared_ptr<Entity>& e) = 0;
		virtual void deleted(const std::shared_ptr<Entity>& e) = 0;
		virtual void enabled(const std::shared_ptr<Entity>& e) = 0;
		virtual void disabled(const std::shared_ptr<Entity>& e) = 0;
	};

	class Manager : public EntityObserver {
	protected:

		World* _world;

	public:

		virtual ~Manager() {

		}

		virtual void initialize() = 0;

		void setWorld(World* world) {
			_world = world;
		}

		World* getWorld() {
			return _world;
		}
		virtual void added(const std::shared_ptr<Entity>& e) {}
		virtual void changed(const std::shared_ptr<Entity>& e) {}
		virtual void deleted(const std::shared_ptr<Entity>& e) {}
		virtual void enabled(const std::shared_ptr<Entity>& e) {}
		virtual void disabled(const std::shared_ptr<Entity>& e) {}
	};

}