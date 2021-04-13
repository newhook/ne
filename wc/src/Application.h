/**
 *
 * Copyright (c) 2014 Matthew Newhook. All rights reserved.
 *
 */
#pragma once

#include <ge/Renderer.h>
#include <ge/Camera.h>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <memory>
#include <random>

namespace sf {
class RenderWindow;
}

namespace wc
{

class Application {
	std::unique_ptr<sf::RenderWindow> _window;

	std::unique_ptr<btDbvtBroadphase> _broadphase;
	std::unique_ptr<btDefaultCollisionConfiguration> _collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> _dispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> _solver;
	std::unique_ptr<btDiscreteDynamicsWorld> _physicsWorld;
	
	ge::Renderer _renderer;

public:

	Application(int argc, char** argv);
	~Application();

	void run();
};

}

 