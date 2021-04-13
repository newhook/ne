#pragma once

#include <artemis/Config.h>
#include <artemis/Manager.h>

#include <unordered_map>
#include <string>

namespace artemis {
	class Entity;

	class TagManager : public Manager {
			std::unordered_map<std::string, std::shared_ptr<Entity> > _tagByEntity;
			std::unordered_map<std::shared_ptr<Entity>, std::string> _entityByTag;

		public:

			TagManager();
			~TagManager();

			virtual void initialize();
			virtual void deleted(const std::shared_ptr<Entity>& e);

			void _register(const std::string& tag, const std::shared_ptr<Entity>& e);
			void unregister(const std::string& tag);
			bool isRegistered(const std::string& tag);
			std::shared_ptr<Entity> getEntity(const std::string& tag) const;
	};
}