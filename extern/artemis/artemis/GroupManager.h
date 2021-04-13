#pragma once

#include <artemis/Manager.h>

#include <unordered_map>
#include <string>

namespace artemis {
	
	class Entity;
	
	class GroupManager : public Manager {
		std::unordered_multimap <std::shared_ptr<Entity>, std::string> _entities;
		std::unordered_multimap <std::string, std::shared_ptr<Entity> > _groups;

	public:

		GroupManager();
		~GroupManager();

		virtual void initialize();

		/**
		 * Set the group of the entity.
		 * 
		 * @param group group to add the entity into.
		 * @param e entity to add into the group.
		 */
		void add(const std::shared_ptr<Entity>& e, const std::string& group);

		/**
		 * Remove the entity from the specified group.
		 * @param e
		 * @param group
		 */
		void remove(const std::shared_ptr<Entity>& e, const std::string& group);

		void removeFromAllGroups(const std::shared_ptr<Entity>& e);

		/**
		 * Get all entities that belong to the provided group.
		 * @param group name of the group.
		 * @return read-only bag of entities belonging to the group.
		 */
		std::pair<std::unordered_multimap <std::string, std::shared_ptr<Entity> >::const_iterator,
			std::unordered_multimap <std::string, std::shared_ptr<Entity> >::const_iterator> getEntities(const std::string& group) const;

		/**
		 * @param e entity
		 * @return the groups the entity belongs to, null if none.
		 */
		std::pair<std::unordered_multimap <std::shared_ptr<Entity>, std::string>::const_iterator,
			std::unordered_multimap <std::shared_ptr<Entity>, std::string>::const_iterator> getGroups(const std::shared_ptr<Entity>& e) const;

		/**
		 * Checks if the entity belongs to any group.
		 * @param e the entity to check.
		 * @return true if it is in any group, false if none.
		 */
		bool isInAnyGroup(const std::shared_ptr<Entity>& e) const;
	
		/**
		 * Check if the entity is in the supplied group.
		 * @param group the group to check in.
		 * @param e the entity to check for.
		 * @return true if the entity is in the supplied group, false if not.
		 */
		bool isInGroup(const std::shared_ptr<Entity>& e, const std::string& group) const;

		virtual void deleted(const std::shared_ptr<Entity>& e);
	};
}