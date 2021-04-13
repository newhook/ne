#pragma once

#include <artemis/Config.h>
#include <artemis/ComponentTypeManager.h>

#include <memory>
#include <vector>

namespace artemis {

	class Component;
	class ComponentType;
	class World;
	
	class Entity : public std::enable_shared_from_this<Entity> {
		int _id;
		World* _world;
		long int _uniqueId;
		std::bitset<BITSIZE> _componentBits;
		std::bitset<BITSIZE> _systemBits;

		std::vector<std::unique_ptr<Component> > _components;

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

	protected:

		friend class EntityManager;
		void reset();

	public:

		Entity(World* world, int id);
		~Entity();

		int getId() const {
			return _id;
		}

		long getUniqueId() const {
			return _uniqueId;
		}

		std::bitset<BITSIZE>& getComponentBits() {
			return _componentBits;
		}

		std::bitset<BITSIZE>& getSystemBits() {
			return _systemBits;
		}

		const std::bitset<BITSIZE>& getSystemBits() const {
			return _systemBits;
		}

		std::string toString();

		/**
		 * Add a component to this entity.
		 *
		 * @param component to add to this entity
		 *
		 * @return this entity for chaining.
		 */
		Entity& addComponent(std::unique_ptr<Component> c);

		/**
		 * Remove component by its type.
		 * @param type
		 *
		 * @return this entity for chaining.
		 */
		Entity& removeComponent(const ComponentType& type);

		/**
		 * Remove component by its type.
		 * @param type
		 *
		 * @return this entity for chaining.
		 */
		template<typename c>
		Entity& removeComponent() {
			removeComponent(ComponentTypeManager::getTypeFor<c>());
			return *this;
		}

		/**
		* Slower retrieval of components from this entity. Minimize usage of this,
		* but is fine to use e.g. when creating new entities and setting data in
		* components.
		*
		* @param <T>
		*            the expected return component type.
		* @param type
		*            the expected return component type.
		* @return component that matches, or null if none is found.
		*/
		Component* getComponent(const ComponentType& type) {
			if (_components.size() >= type.getId()) {
				return _components[type.getId()].get();
			}
			return nullptr;
		}

		/**
		 * Slower retrieval of components from this entity. Minimize usage of this,
		 * but is fine to use e.g. when creating new entities and setting data in
		 * components.
		 *
		 * @param <T>
		 *            the expected return component type.
		 * @param type
		 *            the expected return component type.
		 * @return component that matches, or null if none is found.
		 */
		template<typename T>
		T* getComponent() {
			ComponentType type = ComponentTypeManager::getTypeFor<T>();
			if (_components.size() >= type.getId()) {
				return static_cast<T*>(_components[type.getId()].get());
			}
			return nullptr;
		}

		/**
		 * Checks if the entity has been added to the world and has not been deleted from it.
		 * If the entity has been disabled this will still return true.
		 * 
		 * @return if it's active.
		 */
		bool isActive()  const;

		/**
		 * Will check if the entity is enabled in the world.
		 * By default all entities that are added to world are enabled,
		 * this will only return false if an entity has been explicitly disabled.
		 * 
		 * @return if it's enabled
		 */
		bool isEnabled() const;

		/**
		* This entity has changed, a component added or deleted.
		*/
		void addToWorld();

		/**
		 * This entity has changed, a component added or deleted.
		 */
		void changedInWorld();

		/**
		 * Delete this entity from the world.
		 */
		void deleteFromWorld();
		
		/**
		 * (Re)enable the entity in the world, after it having being disabled.
		 * Won't do anything unless it was already disabled.
		 */
		void enable();
		
		/**
		 * Disable the entity from being processed. Won't delete it, it will
		 * continue to exist but won't get processed.
		 */
		void disable();
	};
}
