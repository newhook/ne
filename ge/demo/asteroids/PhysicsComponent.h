/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>

#include <artemis/Component.h>

class btCollisionShape;
class btRigidBody;

namespace asteroids {

	class PhysicsComponent : public artemis::Component
	{
	public:
		btCollisionShape* shape;
		btRigidBody* rigid;

		// The group to which this component belongs.
		short collisionGroup;
		// The collision mask.
		short collisionMask;

		PhysicsComponent(btCollisionShape* shape, btRigidBody* rigid)
		{
			this->shape = shape;
			this->rigid = rigid;
			this->collisionGroup = 0;
			this->collisionMask = 0;
		}

		PhysicsComponent(btCollisionShape* shape, btRigidBody* rigid, short group, short mask)
		{
			this->shape = shape;
			this->rigid = rigid;
			this->collisionGroup = group;
			this->collisionMask = mask;
		}
	};

}