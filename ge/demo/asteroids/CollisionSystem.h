/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Config.h>

#include <artemis/EntityProcessingSystem.h>
#include <artemis/ComponentMapper.h>
#include <artemis/Entity.h>

class btCollisionDispatcher;

namespace asteroids {

	const short COLL_BOX = 0x01;
	const short COLL_WORLD = 0x02;

	class CollisionSystem : public artemis::EntitySystem
	{
		btCollisionDispatcher* _dispatcher;
		std::list<std::pair<artemis::Entity*, artemis::Entity*> > _pairs;

	public:

		CollisionSystem(btCollisionDispatcher* dispatcher);

		virtual void initialize();

		virtual bool checkProcessing();

		virtual void begin();

		virtual void end();

		virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag);
	};
}
