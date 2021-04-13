#include <artemis/Config.h>
#include <artemis/SystemBitManager.h>

#include <unordered_map>

using namespace artemis;

namespace {
static int POS = 0;
static std::unordered_map< size_t, std::bitset<BITSIZE>* > systemBits;

}
void SystemBitManager::removeBitSets(){
	std::unordered_map< size_t, std::bitset<BITSIZE>* >::iterator it;
	
	for(it = systemBits.begin(); it != systemBits.end(); it++)
	{
		delete it->second;
	}
}

std::bitset<BITSIZE> & SystemBitManager::getBitFor(const std::type_info & type) {

	//assert((std::is_base_of< EntitySystem, system >::value == true));

	size_t hash = type.hash_code();

	std::bitset<BITSIZE> * bit = systemBits[hash];

	if(bit == nullptr) {

		bit = new std::bitset<BITSIZE>(1);
		(*bit)  <<=  POS++;

		systemBits[hash] = bit;
	}

	return *bit;

}
