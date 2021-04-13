#include <artemis/Config.h>
#include <artemis/GroupManager.h>
#include <artemis/Entity.h>

using namespace artemis;
using namespace std;

GroupManager::GroupManager() {

}

GroupManager::~GroupManager() {

}

void
GroupManager::initialize() {
}

void
GroupManager::add(const std::shared_ptr<Entity>& e, const string& group)
{
	_entities.insert(make_pair(e, group));
	_groups.insert(make_pair(group, e));
}

void
GroupManager::remove(const std::shared_ptr<Entity>& e, const string& group)
{
	{
		auto range = _entities.equal_range(e);
		for (auto p = range.first; p != range.second; ++p) {
			if (p->second == group) {
				_entities.erase(p);
				break;
			}
		}
	}

	{
		auto range = _groups.equal_range(group);
		for (auto p = range.first; p != range.second; ++p) {
			if (p->second == e) {
				_groups.erase(p);
				break;
			}
		}
	}
}

void 
GroupManager::removeFromAllGroups(const std::shared_ptr<Entity>& e) {
	auto range = _entities.equal_range(e);
	for (auto p = range.first; p != range.second;) {
		auto r2 = _groups.equal_range(p->second);
		for (auto q = r2.first; q != r2.second; ++q) {
			if (q->second == e) {
				_groups.erase(q);
				break;
			}
		}
		_entities.erase(p++);
	}
}

pair<unordered_multimap <string, std::shared_ptr<Entity> >::const_iterator,
	 unordered_multimap <string, std::shared_ptr<Entity> >::const_iterator> 
GroupManager::getEntities(const string& group) const
{
	return _groups.equal_range(group);
}

pair<unordered_multimap <std::shared_ptr<Entity>, string>::const_iterator,
	unordered_multimap <std::shared_ptr<Entity>, string>::const_iterator>
GroupManager::getGroups(const std::shared_ptr<Entity>& e)  const
{
	return _entities.equal_range(e);
}

bool
GroupManager::isInAnyGroup(const std::shared_ptr<Entity>& e)  const {
	auto p = getGroups(e);
	return p.first != p.second;
}

bool 
GroupManager::isInGroup(const std::shared_ptr<Entity>& e, const string& group) const  {
	auto range = _entities.equal_range(e);
	for (auto p = range.first; p != range.second; ++p) {
		if (p->second == group) {
			return true;
		}
	}
	return false;
}

void
GroupManager::deleted(const std::shared_ptr<Entity>& e) {
	removeFromAllGroups(e);
}
