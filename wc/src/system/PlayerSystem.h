/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <artemis/Artemis.h>

#include <component/PlayerComponent.h>
#include <component/PathComponent.h>
#include <component/TransformComponent.h>
#include <component/PhysicsComponent.h>

namespace ge
{
class Camera;
}

namespace sf
{
class Window;
}

namespace wc {
	class TerrainComponent;

	// TODO: Bullet and such should live in the physics system.
	class PlayerSystem : public artemis::EntityProcessingSystem
	{
		artemis::ComponentMapper<PlayerComponent> playerMapper;
		artemis::ComponentMapper<PathComponent> pathMapper;

		artemis::ComponentMapper<TransformComponent> transformMapper;
		artemis::ComponentMapper<PhysicsComponent> physicsMapper;

		sf::Window* _window;
		ge::Camera* _camera;
		bool _reset;

		std::shared_ptr<artemis::Entity> _terrain;

	public:

		PlayerSystem(sf::Window* window);

		virtual void initialize();

		virtual void processEntity(const std::shared_ptr<artemis::Entity>& e);

		void reset() { _reset = true;  }
	};
}