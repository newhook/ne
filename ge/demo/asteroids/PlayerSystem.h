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

#include <asteroids/PlayerComponent.h>
#include <asteroids/TransformComponent.h>
#include <asteroids/PhysicsComponent.h>

namespace ge
{
class Camera;
}

namespace sf
{
class Window;
}

namespace asteroids {

	// TODO: Bullet and such should live in the physics system.
	class PlayerSystem : public artemis::EntityProcessingSystem
	{
		artemis::ComponentMapper<PlayerComponent> playerMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;
		artemis::ComponentMapper<PhysicsComponent> physicsMapper;

		sf::Window* _window;
		ge::Camera* _camera;
		bool _reset;

	public:

		PlayerSystem(sf::Window* window);

		virtual void initialize();

		virtual void processEntity(artemis::Entity &e);

		void reset() { _reset = true;  }
	};
}