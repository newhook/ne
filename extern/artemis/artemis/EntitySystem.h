#pragma once

#include <artemis/Config.h>
#include <artemis/Aspect.h>
#include <artemis/Component.h>
#include <artemis/Manager.h>
#include <artemis/ComponentTypeManager.h>

#include <iostream>
#include <assert.h>
#include <typeinfo>
#include <bitset>
#include <vector>
#include <memory>

namespace artemis {

	class Entity;
	class World;
	
	class EntitySystem : public EntityObserver {
		std::bitset<BITSIZE> _systemBit;

		Aspect _aspect;
		std::bitset<BITSIZE> _allSet;
		std::bitset<BITSIZE> _exclusionSet;
		std::bitset<BITSIZE> _oneSet;

		std::vector<std::shared_ptr<Entity> > _actives;

		bool _passive;

		void removeFromSystem(const std::shared_ptr<Entity>& e);
		void insertToSystem(const std::shared_ptr<Entity>& e);
		void check(const std::shared_ptr<Entity>& e);

	public:

		const std::bitset<BITSIZE>& getSystemBit() const {
			return _systemBit;
		}

		void setSystemBit(const std::bitset<BITSIZE>& bit) {
			_systemBit = bit;
		}

		virtual ~EntitySystem();

		/*override these functions*/
		virtual void initialize() {};

		void setWorld(World *w) {
			world = w;
		}
		void change(Entity &e);
		void process();
		int getEntityCount() const {
			return (int)_actives.size();
		}

		virtual void added(const std::shared_ptr<Entity>& e);
		virtual void changed(const std::shared_ptr<Entity>& e);
		virtual void deleted(const std::shared_ptr<Entity>& e);
		virtual void enabled(const std::shared_ptr<Entity>& e);
		virtual void disabled(const std::shared_ptr<Entity>& e);

		bool isPassive() const {
			return _passive;
		}
		void setPassive(bool v) {
			_passive = v;
		}

	protected:

		EntitySystem(const Aspect& aspect);

		World * world;
			

		/*override these functions*/
		virtual void begin();
		virtual void end();

		/**
		 * Called if the system has received a entity it is interested in, e.g. created or a component was added to it.
		 * @param e the entity that was added to this system.
		 */
		virtual void inserted(const std::shared_ptr<Entity>& e);

		/**
		 * Called if a entity was removed from this system, e.g. deleted or had one of it's components removed.
		 * @param e the entity that was removed from this system.
		 */
		virtual void removed(const std::shared_ptr<Entity>& e);


		// Abstracts
		virtual void processEntities(const std::vector<std::shared_ptr<Entity> > & entities) = 0;
		virtual bool checkProcessing() = 0;
	};

}
