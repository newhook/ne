/**
 *
 * Copyright (c) 2014 Matthew Newhook. All rights reserved.
 *
 */

#include <Config.h>
#include <Application.h>

#include <SFML/Graphics.hpp>

#include <system/PhysicsSystem.h>
#include <system/AnimationSystem.h>
#include <system/RenderSystem.h>
#include <system/PlayerSystem.h>
#include <system/ExpirationSystem.h>
#include <system/PathFinderSystem.h>
#include <system/PathSystem.h>
#include <system/CreepSystem.h>
#include <system/TowerSystem.h>
#include <system/ProjectileSystem.h>
#include <system/EffectSystem.h>
#include <component/TerrainComponent.h>
#include <Resource.h>

//#include <system/CollisionSystem.h>
#include <EntityFactory.h>
#include <bullet/BulletDebug.h>


#include <ge/gl.h>
#include <ge/Mesh.h>
#include <ge/ShaderProgram.h>
#include <ge/InputStream.h>
#include <ge/PGeom.h>
#include <ge/Logger.h>

using namespace std;
using namespace wc;

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

	ge::Logger::configureLevel("", ge::LoggerLevel::Warn);
	ge::Logger::configureAppender("A", "DebugAppender");
	ge::Logger::reconfig();

	_window = unique_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(600, 600), "asteroids"));

	GLenum status = glewInit();
	if (status != GLEW_OK) {
		// XXX:
		int a = 0;
	}

	ge::FileInputStream input;
	input.open("shaders/shaders.json");
	ge::ShaderProgram::initialize(input);

	wc::resource::addTextureLocation("models");
	wc::resource::addTextureLocation("models/Enemies/textures");
	wc::resource::addTextureLocation("models/Tower/textures");
	wc::resource::addTextureLocation("models/World/textures");
	wc::resource::addTextureLocation("models/Projectile/textures");

	_broadphase = make_unique<btDbvtBroadphase>();
	_collisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	_dispatcher = make_unique<btCollisionDispatcher>(_collisionConfiguration.get());
	_solver = make_unique<btSequentialImpulseConstraintSolver>();
	_physicsWorld = make_unique<btDiscreteDynamicsWorld>(_dispatcher.get(), _broadphase.get(), _solver.get(), _collisionConfiguration.get());
	_physicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

Application::~Application() {
}

void
Application::run() {
	artemis::World world;

	world.setManager(make_unique<artemis::GroupManager>());
	world.setManager(make_unique<artemis::TagManager>());
	EntityFactory* factory = world.setManager(make_unique<EntityFactory>());

	PhysicsSystem* physicsSystem = world.setSystem(make_unique<PhysicsSystem>(_physicsWorld.get()));
	AnimationSystem* animationSystem = world.setSystem(make_unique<AnimationSystem>());
	RenderSystem* renderSystem = world.setSystem(make_unique<RenderSystem>(&_renderer, _window->getSize().x, _window->getSize().y));
	PlayerSystem* playerSystem = world.setSystem(make_unique<PlayerSystem>(_window.get()));
	ExpirationSystem* expirationSystem = world.setSystem(make_unique<ExpirationSystem>());
	CreepSystem* creepSystem = world.setSystem(make_unique<CreepSystem>());
	TowerSystem* towerSystem = world.setSystem(make_unique<TowerSystem>());
	PathFinderSystem* pathFinderSystem = world.setSystem(make_unique<PathFinderSystem>());
	PathSystem* pathSystem = world.setSystem(make_unique<PathSystem>());
	ProjectileSystem* projectileSystem = world.setSystem(make_unique<ProjectileSystem>());
	EffectSystem* effectSystem = world.setSystem(make_unique<EffectSystem>());

	//CollisionSystem& collisionSystem = world.setSystem(make_unique<CollisionSystem>(_dispatcher.get()));

	world.initialize();

	bool u = false;
	shared_ptr<artemis::Entity> uni = factory->createUniverse();
	uni->addToWorld();
	pathFinderSystem->setTerrain(uni);
	
	BulletDebugDrawer debugDrawer(*renderSystem);
	debugDrawer.setDebugMode(btIDebugDraw::DBG_NoDebug);

	_physicsWorld->setDebugDrawer(&debugDrawer);

	sf::Vector2i center;
	center.x = _window->getSize().x / 2;
	center.y = _window->getSize().y / 2;

	factory->createPlayer(glm::vec3(10.0f, 10.0f, 10.0f), 0)->addToWorld();
	
	// Declare and load a font
	sf::Font font;
	font.loadFromFile("fonts/OpenSans-Regular.ttf");
	// Create a text
	sf::Text text("hello", font);
	text.setCharacterSize(30);
	text.setStyle(sf::Text::Bold);
	text.setColor(sf::Color::Red);

	factory->createTower(glm::vec3(0, 0, 10), 0)->addToWorld();
	factory->createTower(glm::vec3(0, 0, -10), 0)->addToWorld();

	sf::Clock clock;
	while (_window->isOpen()) {
		float time = clock.restart().asSeconds();
		_physicsWorld->stepSimulation(time, 10);

		world.setDelta(time);
		world.process();

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
					renderSystem->setShowScene(!renderSystem->getShowScene());
					break;

				case sf::Keyboard::R:
					//playerSystem.reset();
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
					renderSystem->setShowBounds(!renderSystem->getShowBounds());
					break;

				case sf::Keyboard::F:
					renderSystem->setWireframe(!renderSystem->getWireframe());
					break;
				}
				break;
			}
		}
		
		_renderer.clearBuffers(ge::Renderer::BUFFER_COLOR_AND_DEPTH);

		// No need to clear the window, the renderer.clearBuffers does that.
		//_window->clear();

		_physicsWorld->debugDrawWorld();

		renderSystem->process();

		_renderer.endFrame();

		_window->resetGLStates();
		_window->draw(text);

		// TODO: After this the SFML opengl states are set. We need to restore or clear our render context.

		//_window->popGLStates();

		_window->display();

		// TODO: Horrific.
		//if (appInFocus(_window.get())) {
		//	sf::Mouse::setPosition(center, *_window.get());
		//}
	}
}
