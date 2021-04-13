#pragma once

#include <artemis/Config.h>

#include <bitset>

namespace artemis {

	class Aspect {
	protected:

		std::bitset<BITSIZE> _allSet;
		std::bitset<BITSIZE> _exclusionSet;
		std::bitset<BITSIZE> _oneSet;

		Aspect() {

		}

	public:

		const std::bitset<BITSIZE>& getAllSet() const {
			return _allSet;
		}
		const std::bitset<BITSIZE>& getExclusionSet() const {
			return _exclusionSet;
		}
		const std::bitset<BITSIZE>& getOneSet() const {
			return _oneSet;
		}

		//============================================================
		//Meta templates. Helps with passing each type from the variadic template
		/*
		*Struct to pass rest the types
		*/
		template<typename...>
		struct typelist {};

		/**
		* Returns an aspect where an entity must possess all of the specified component types.
		* @param type a required component type
		* @param types a required component type
		* @return an aspect that can be matched against entities
		*/
		/*
		* Recursively called. On each iteration  "component" is set to the next "typename" from rest pack set. .
		* We pass this component into our ComponentManager.
		*/
		template<typename...components>
		Aspect& all() {
			all(typelist<components...>());
			return *this;
		}

		/**
		* Excludes all of the specified component types from the aspect. A system will not be
		* interested in an entity that possesses one of the specified exclusion component types.
		*
		* @param type component type to exclude
		* @param types component type to exclude
		* @return an aspect that can be matched against entities
		*/
		template<typename...components>
		Aspect& exclude() {
			exclude(typelist<components...>());
			return *this;
		}

		/**
		* Returns an aspect where an entity must possess one of the specified component types.
		* @param type one of the types the entity must possess
		* @param types one of the types the entity must possess
		* @return an aspect that can be matched against entities
		*/
		template<typename...components>
		Aspect& one() {
			one(typelist<components...>());
			return *this;
		}

		/**
		* Creates an aspect where an entity must possess all of the specified component types.
		*
		* @param type the type the entity must possess
		* @param types the type the entity must possess
		* @return an aspect that can be matched against entities
		*
		* @deprecated
		* @see getAspectForAll
		*/
		template<typename...components>
		static Aspect getAspectFor() {
			Aspect aspect;
			aspect.all(typelist<components...>());
			return aspect;
		}

		/**
		* Creates an aspect where an entity must possess all of the specified component types.
		*
		* @param type a required component type
		* @param types a required component type
		* @return an aspect that can be matched against entities
		*/
		template<typename...components>
		static Aspect getAspectForAll() {
			Aspect aspect;
			aspect.all(typelist<components...>());
			return aspect;
		}

		/**
		* Creates an aspect where an entity must possess one of the specified component types.
		*
		* @param type one of the types the entity must possess
		* @param types one of the types the entity must possess
		* @return an aspect that can be matched against entities
		*/
		template<typename...components>
		static Aspect getAspectForOne() {
			Aspect aspect;
			aspect.one(typelist<components...>());
			return aspect;
		}

		/**
		* Creates and returns an empty aspect. This can be used if you want a system that processes no entities, but
		* still gets invoked. Typical usages is when you need to create special purpose systems for debug rendering,
		* like rendering FPS, how many entities are active in the world, etc.
		*
		* You can also use the all, one and exclude methods on this aspect, so if you wanted to create a system that
		* processes only entities possessing just one of the components A or B or C, then you can do:
		* Aspect.getEmpty().one(A,B,C);
		*
		* @return an empty Aspect that will reject all entities.
		*/
		static Aspect getEmpty() {
			return Aspect();
		}

	private:

		template<typename component, typename ... Rest>
		void all(typelist<component, Rest...>) {
			_allSet |= ComponentTypeManager::getBit<component>();
			all(typelist<Rest...>());
		}

		void all(typelist<>) {
		}

		template<typename component, typename ... Rest>
		void exclude(typelist<component, Rest...>) {
			_exclusionSet |= ComponentTypeManager::getBit<component>();
			exclude(typelist<Rest...>());
		}
		void exclude(typelist<>) {
		}

		template<typename component, typename ... Rest>
		void one(typelist<component, Rest...>) {
			_oneSet |= ComponentTypeManager::getBit<component>();
			one(typelist<Rest...>());
		}
		void one(typelist<>) {
		}

	};
};

