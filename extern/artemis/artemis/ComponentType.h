#pragma once

#include <artemis/Config.h>

#include <bitset>

namespace artemis {
	class ComponentType {
		std::bitset<BITSIZE> _bit;
		size_t _id;

	public:

		ComponentType(const ComponentType&) = default;

		ComponentType(const std::bitset<BITSIZE> &bit, size_t id) : _bit(bit), _id(id) {

		}

		const std::bitset<BITSIZE>& getBit() const {
			return _bit;
		}
		size_t getId() const {
			return _id;
		}
	};
}