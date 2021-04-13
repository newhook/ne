#include <artemis/Config.h>
#include <artemis/TagManager.h>
#include <artemis/Entity.h>

using namespace artemis;

TagManager::TagManager() {

}

TagManager::~TagManager() {

}

void TagManager::initialize() {}

std::shared_ptr<Entity> TagManager::getEntity(const std::string& tag) const {
	auto p = _tagByEntity.find(tag);
	if (p != _tagByEntity.end()) {
		return p->second;
	}
	return 0;
}

bool TagManager::isRegistered(const std::string& tag) {
	return _tagByEntity.find(tag) != _tagByEntity.end();
}

void TagManager::deleted(const std::shared_ptr<Entity>& e) {
	auto p = _entityByTag.find(e);
	if (p != _entityByTag.end()) {
		_tagByEntity.erase(p->second);
		_entityByTag.erase(p);
	}
}

void TagManager::unregister(const std::string& tag) {
	auto p = _tagByEntity.find(tag);
	if (p != _tagByEntity.end()) {
		_entityByTag.erase(p->second);
		_tagByEntity.erase(p);
	}
}
	
void TagManager::_register(const std::string& tag, const std::shared_ptr<Entity>& e){
	{
		auto p = _tagByEntity.find(tag);
		if (p == _tagByEntity.end()) {
			_tagByEntity.insert(make_pair(tag, e));
		}
		else {
			p->second = e;
		}
	}
	{
		auto p = _entityByTag.find(e);
		if (p == _entityByTag.end()) {
			_entityByTag.insert(make_pair(e, tag));
		}
		else {
			p->second = tag;
		}
	}

}

