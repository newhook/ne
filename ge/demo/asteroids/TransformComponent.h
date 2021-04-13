/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <asteroids/Config.h>
#include <ge/Transform.h>
#include <ge/MeshR.h>

#include <artemis/Component.h>

#include <vector>
#include <memory>

class btCollisionShape;
class btRigidBody;

namespace asteroids {

	class TransformComponent : public artemis::Component
	{
	public:

		TransformComponent(const std::vector<std::shared_ptr<ge::MeshR> >& renderers, const ge::Transform& t) {
			for (auto p = renderers.begin(); p != renderers.end(); ++p) {
				localTransform.push_back((*p)->getTransform());
			}
			transform = t;
		}

		TransformComponent(const ge::Transform& t) {
			transform = t;
		}

		ge::Transform getWorldTransform(size_t i) const;
		ge::Transform getWorldTransform() const;

		std::vector<ge::Transform> localTransform;
		ge::Transform transform;
	};

}