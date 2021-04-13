#include <artemis/ComponentTypeManager.h>

#include <memory>
#include <unordered_map>

using namespace artemis;
using namespace std;

namespace {

unordered_map<string, unique_ptr<ComponentType> > componentTypes;

std::bitset<BITSIZE> nextBit(1);
size_t nextId = 0;

}

ComponentType& ComponentTypeManager::getTypeFor(const type_info &t) {
	auto p = componentTypes.find(t.name());
	if (p != componentTypes.end()) {
		return *p->second;
	}

	std::bitset<BITSIZE> bit = nextBit;
	nextBit = nextBit << 1;
	size_t id = nextId++;
	auto q = componentTypes.insert(make_pair(t.name(), make_unique<ComponentType>(bit, id)));
	return *q.first->second;
}

void ComponentTypeManager::deleteComponentTypes(){
	componentTypes.clear();
}


