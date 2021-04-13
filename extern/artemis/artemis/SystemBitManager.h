#pragma once

#include <artemis/Config.h>

#include <typeinfo>
#include <bitset>

namespace artemis {
	
	class EntitySystem;
	
	class SystemBitManager {
		public:

			static std::bitset<BITSIZE> & getBitFor(const std::type_info & type);
			static void removeBitSets();
			template<typename system>
			static std::bitset<BITSIZE> & getBitFor() {

				assert((std::is_base_of< EntitySystem, system >::value == true));

				return getBitFor(typeid(system));

			}
			
	};
}
