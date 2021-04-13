#pragma once

#include <artemis/ComponentTypeManager.h>
#include <artemis/Entity.h>

namespace artemis {
	template<typename T>
	class ComponentMapper {
		ComponentType _type;

	public:

		ComponentMapper() : _type(ComponentTypeManager::getTypeFor<T>()) {
		}

		~ComponentMapper() {
		}

		T* get(const std::shared_ptr<Entity>& e) {
			return (T*)e->getComponent(_type);
		}
	};
}
