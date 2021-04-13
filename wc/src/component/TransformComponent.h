/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <Config.h>
#include <ge/Transform.h>
#include <ge/MeshR.h>
#include <ge/SkinMeshR.h>

#include <artemis/Artemis.h>

#include <vector>
#include <memory>

class btCollisionShape;
class btRigidBody;

namespace wc {

	class TransformComponent : public artemis::Component
	{
	public:

		TransformComponent(const std::vector<std::shared_ptr<ge::MeshR> >& renderers, const ge::Transform& t) {
			for (auto p : renderers) {
				localTransform.push_back(p->getTransform());
				names.push_back(p->getName());
			}
			transform = t;
		}

		TransformComponent(const std::vector<std::shared_ptr<ge::SkinMeshR> >& renderers, const ge::Transform& t) {
			for (auto p : renderers) {
				localTransform.push_back(p->getTransform());
				names.push_back(p->getName());
			}
			transform = t;
		}
		TransformComponent(const ge::Transform& t) {
			transform = t;
		}

		ge::Transform getWorldTransform(size_t i) const;
		ge::Transform getWorldTransform() const;

		std::vector<std::string> names;
		std::vector<ge::Transform> localTransform;
		ge::Transform transform;
	};

}