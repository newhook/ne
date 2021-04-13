/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <btBulletDynamicsCommon.h>

namespace wc {

	class PhysicsComponent : public artemis::Component
	{
	public:
		std::unique_ptr<btCollisionShape> shape;
		std::unique_ptr<btRigidBody> rigid;

		// The group to which this component belongs.
		short collisionGroup;
		// The collision mask.
		short collisionMask;

		PhysicsComponent(std::unique_ptr<btCollisionShape> shape, std::unique_ptr<btRigidBody> rigid) : shape(std::move(shape)), rigid(std::move(rigid))
		{
			this->collisionGroup = 0;
			this->collisionMask = 0;
		}

		PhysicsComponent(std::unique_ptr<btCollisionShape> shape, std::unique_ptr<btRigidBody> rigid, short group, short mask) : shape(std::move(shape)), rigid(std::move(rigid))
		{
			this->collisionGroup = group;
			this->collisionMask = mask;
		}
	};

}