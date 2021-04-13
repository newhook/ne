#pragma once

#include <artemis/Component.h>
#include <artemis/ComponentTypeManager.h>
#include <artemis/Manager.h>

#include <memory>
#include <vector>
#include <set>

namespace artemis {
	class EntityManager : public Manager {
		std::vector<std::shared_ptr<Entity> > _entities;
		std::set<int> _disabled;
		std::set<int> _clean;

		int _active;
		long _added;
		long _created;
		long _deleted;

		int _nextAvailableId;
		std::vector<int> _availableIds;

	public:

		EntityManager();
		~EntityManager();

		virtual void initialize() {

		}
		std::shared_ptr<Entity> createEntityInstance();

		void clean();

		virtual void added(const std::shared_ptr<Entity>& e);
		virtual void changed(const std::shared_ptr<Entity>& e);
		virtual void deleted(const std::shared_ptr<Entity>& e);
		virtual void enabled(const std::shared_ptr<Entity>& e);
		virtual void disabled(const std::shared_ptr<Entity>& e);

		/**
		* Check if this entity is active.
		* Active means the entity is being actively processed.
		*
		* @param entityId
		* @return true if active, false if not.
		*/
		bool isActive(int entityId);

		/**
		* Check if the specified entityId is enabled.
		*
		* @param entityId
		* @return true if the entity is enabled, false if it is disabled.
		*/
		bool isEnabled(int entityId);

		/**
		* Get a entity with this id.
		*
		* @param entityId
		* @return the entity
		*/
		std::shared_ptr<Entity> getEntity(int entityId) const;

		/**
		* Get how many entities are active in this world.
		* @return how many entities are currently active.
		*/
		int getActiveEntityCount() const {
			return _active;
		}

		/**
		* Get how many entities have been created in the world since start.
		* Note: A created entity may not have been added to the world, thus
		* created count is always equal or larger than added count.
		* @return how many entities have been created since start.
		*/
		long getTotalCreated() const {
			return _created;
		}

		/**
		* Get how many entities have been added to the world since start.
		* @return how many entities have been added.
		*/
		long getTotalAdded() const {
			return _added;
		}

		/**
		* Get how many entities have been deleted from the world since start.
		* @return how many entities have been deleted since start.
		*/
		long getTotalDeleted() const {
			return _deleted;
		}
	};
};
