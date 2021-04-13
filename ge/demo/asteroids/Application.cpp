/**
 *
 * Copyright (c) 2014 Matthew Newhook. All rights reserved.
 *
 */

#include <asteroids/Application.h>

#include <SFML/Graphics.hpp>

#include <asteroids/PhysicsSystem.h>
#include <asteroids/RenderSystem.h>
#include <asteroids/PlayerSystem.h>
#include <asteroids/ExpirationSystem.h>
#include <asteroids/CollisionSystem.h>
#include <asteroids/EntityFactory.h>
#include <asteroids/BulletDebug.h>

#include <artemis/World.h>
#include <artemis/SystemManager.h>
#include <artemis/EntityManager.h>
#include <artemis/EntityProcessingSystem.h>
#include <artemis/ComponentMapper.h>
#include <artemis/Entity.h>

#include <ge/gl.h>
#include <ge/Mesh.h>
#include <ge/ShaderProgram.h>
#include <ge/PGeom.h>

using namespace std;
using namespace asteroids;

namespace {
	bool appInFocus(sf::RenderWindow* app) {
		if (app == NULL)
			return false;

		HWND handle = app->getSystemHandle();
		bool one = handle == GetFocus();
		bool two = handle == GetForegroundWindow();

		if (one != two) //strange 'half-focus': window is in front but you can't click anything - so we fix it
		{
			SetFocus(handle);
			SetForegroundWindow(handle);
		}

		return one && two;
	}
}

Application::Application(int argc, char** argv) {

	_window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(600, 600), "asteroids"));

	GLenum status = glewInit();
	if (status != GLEW_OK) {
		// XXX:
		int a = 0;
	}

	_broadphase = unique_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
	_collisionConfiguration = unique_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
	_dispatcher = unique_ptr<btCollisionDispatcher>(new btCollisionDispatcher(_collisionConfiguration.get()));
	_solver = unique_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
	_physicsWorld = unique_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(_dispatcher.get(), _broadphase.get(), _solver.get(), _collisionConfiguration.get()));
	//_physicsWorld->setGravity(btVector3(0, -9.8f, 0));
	_physicsWorld->setGravity(btVector3(0, 0, 0));

}

Application::~Application() {
}

void
Application::run() {
	artemis::World world;
	artemis::SystemManager * sm = world.getSystemManager();
	PhysicsSystem& physicsSystem = (PhysicsSystem&)sm->setSystem(new PhysicsSystem(_physicsWorld.get()));
	RenderSystem& renderSystem = (RenderSystem&)sm->setSystem(new RenderSystem(&_renderer, _window->getSize().x, _window->getSize().y));
	PlayerSystem& playerSystem = (PlayerSystem&)sm->setSystem(new PlayerSystem(_window.get()));
	ExpirationSystem& expirationSystem = (ExpirationSystem&)sm->setSystem(new ExpirationSystem());
	CollisionSystem& collisionSystem = (CollisionSystem&)sm->setSystem(new CollisionSystem(_dispatcher.get()));
	artemis::EntityManager * em = world.getEntityManager();

	sm->initializeAll();

	// Create the world box.
	EntityFactory factory(&world);
	factory.createUniverse();

	BulletDebugDrawer debugDrawer(renderSystem);
	debugDrawer.setDebugMode(btIDebugDraw::DBG_NoDebug);

	_physicsWorld->setDebugDrawer(&debugDrawer);

	unsigned seed = 1234;
	std::mt19937 random(seed);
	size_t maxAsteroids = 10;
	for (size_t i = 0; i < maxAsteroids; ++i) {
		float x = 1 + ((float)random() / random.max()) * 48.0f;
		float z = 1 + ((float)random() / random.max()) * 48.0f;
		float y = 1 + ((float)random() / random.max()) * 48.0f;
		factory.createAsteroid(glm::vec3(x, y, z));
	}

	sf::Vector2i center;
	center.x = _window->getSize().x / 2;
	center.y = _window->getSize().y / 2;

	factory.createPlayer(glm::vec3(0.0f));

	sf::Clock clock;
	while (_window->isOpen()) {
		float time = clock.restart().asSeconds();
		_physicsWorld->stepSimulation(time, 10);

		world.loopStart();
		world.setDelta(time);

		physicsSystem.process();
		playerSystem.process();
		collisionSystem.process();
		expirationSystem.process();

		sf::Event event;
		while (_window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				_window->close();
			}
			switch (event.type) {
			case sf::Event::KeyReleased:
				switch (event.key.code) {
					// Switch cameras.
				// Show scene geometry.
				case sf::Keyboard::G:
					renderSystem.setShowScene(!renderSystem.getShowScene());
					break;

				case sf::Keyboard::R:
					playerSystem.reset();
					break;

				// Show physics geom.
				case sf::Keyboard::P:
					if (debugDrawer.getDebugMode() == btIDebugDraw::DBG_DrawWireframe) {
						debugDrawer.setDebugMode(btIDebugDraw::DBG_NoDebug);
					}
					else {
						debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
					}
					break;

				case sf::Keyboard::B:
					renderSystem.setShowBounds(!renderSystem.getShowBounds());
					break;

				case sf::Keyboard::F:
					renderSystem.setWireframe(!renderSystem.getWireframe());
					break;
				}
				break;
			}
		}

		_renderer.clearBuffers(ge::Renderer::BUFFER_COLOR_AND_DEPTH);

		_window->clear();

		_physicsWorld->debugDrawWorld();

		renderSystem.process();

		_window->display();

		// TODO: Horrific.
		//if (appInFocus(_window.get())) {
		//	sf::Mouse::setPosition(center, *_window.get());
		//}
	}
}
