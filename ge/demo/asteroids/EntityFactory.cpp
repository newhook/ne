/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/EntityFactory.h>
#include <asteroids/PhysicsComponent.h>
#include <asteroids/RenderComponent.h>
#include <asteroids/TransformComponent.h>
#include <asteroids/PlayerComponent.h>
#include <asteroids/CollisionSystem.h>
#include <asteroids/BulletUtil.h>
#include <asteroids/RenderSystem.h>
#include <asteroids/Resource.h>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <artemis/SystemManager.h>

#include <ge/Renderer.h>
#include <ge/Mesh.h>
#include <ge/ShaderProgram.h>
#include <ge/PGeom.h>
#include <ge/MeshR.h>
#include <ge/Material.h>

#include <ge/InputStream.h>

#include <sstream>
#include <random>

using namespace asteroids;
using namespace std;

namespace {
	shared_ptr<sf::Texture>
		loadTexture(const string& name) {
		ge::FileInputStream stream;
		if (!stream.open(name)) {
			printf("cannot open `%s'", name);
			return nullptr;
		}
		shared_ptr<sf::Texture> texture(new sf::Texture());
		if (!texture->loadFromStream(stream)) {
			printf("cannot load `%s'", name);
			return nullptr;
		}
		return texture;
	}

	std::vector< shared_ptr<ge::MeshR > > _asteroids;
	std::vector< shared_ptr<sf::Texture> > _textures;
	shared_ptr<ge::Cubemap> _skybox;
}


EntityFactory::EntityFactory(artemis::World* world) : _world(world) {
	//size_t max = 30;
	size_t max = 1;
	for (size_t i = 1; i < max + 1; ++i) {
		ostringstream os;
		os << "models/obj/asteroid" << i << ".obj";
		std::vector<shared_ptr<ge::MeshR> > a1 = resource::loadobj(os.str(), "asteroid");
		if (!a1.empty()) {
			for (auto p = a1.begin(); p != a1.end(); ++p) {
				(*p)->setTransform((*p)->getTransform().setScale(0.3f));
				_asteroids.push_back(*p);
			}
		}
	}

	//max = 15;
	max = 1;
	for (size_t i = 1; i < max + 1; ++i) {
		ostringstream os;
		os << "models/textures/am" << i << ".jpg";

		ge::FileInputStream stream;
		if (!stream.open(os.str())) {
			printf("cannot open %s", os.str());
			continue;
		}
		shared_ptr<sf::Texture> texture(loadTexture(os.str()));
		if (texture != nullptr) {
			_textures.push_back(texture);
		}
	}

	_skybox = shared_ptr<ge::Cubemap>(new ge::Cubemap());
	vector<string> names;
	names.resize(6);

	// GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	names[1] = "models/textures/skybox_left2.png";
	names[0] = "models/textures/skybox_right1.png";
	names[3] = "models/textures/skybox_top3.png";
	names[2] = "models/textures/skybox_bottom4.png";
	names[5] = "models/textures/skybox_front5.png";
	names[4] = "models/textures/skybox_back6.png";

	//names[0] = "models/textures/negx.jpg";
	//names[1] = "models/textures/posx.jpg";
	//names[2] = "models/textures/negy.jpg";
	//names[3] = "models/textures/posy.jpg";
	//names[4] = "models/textures/negz.jpg";
	//names[5] = "models/textures/posz.jpg";

	if (!_skybox->load(names)) {
		printf("cannot open skybox");
	}
}


// Create the world box.
artemis::Entity& EntityFactory::createUniverse()
{
	artemis::EntityManager* em = _world->getEntityManager();
	RenderSystem& renderSystem = (RenderSystem&)_world->getSystemManager()->getSystem<RenderSystem>();

	shared_ptr<ge::Mesh> skybox(new ge::Mesh());

	const float points[] = {
		-10.0f, 10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, 10.0f, -10.0f,
		-10.0f, 10.0f, -10.0f,

		-10.0f, -10.0f, 10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f, 10.0f, -10.0f,
		-10.0f, 10.0f, -10.0f,
		-10.0f, 10.0f, 10.0f,
		-10.0f, -10.0f, 10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f, 10.0f,
		-10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, -10.0f, 10.0f,
		-10.0f, -10.0f, 10.0f,

		-10.0f, 10.0f, -10.0f,
		10.0f, 10.0f, -10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		-10.0f, 10.0f, 10.0f,
		-10.0f, 10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f, 10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f, 10.0f,
		10.0f, -10.0f, 10.0f
	};
	size_t npoints = sizeof(points) / sizeof(points[0]);

	vector<glm::vec3>& vertices = skybox->vertices;
	vertices.resize(npoints / 3);
	size_t v = 0;
	for (size_t i = 0; i < npoints; i += 3) {
		vertices[v++] = glm::vec3(points[i], points[i + 1], points[i + 2]);
	}

	shared_ptr<ge::MeshR> skyboxRenderer(new ge::MeshR());
	skyboxRenderer->setMesh(skybox);
	skyboxRenderer->setCamera(renderSystem.getSkyboxCamera());
	skyboxRenderer->setMaterial(ge::Material(ge::ShaderProgram::find("uni_outer")));
	skyboxRenderer->getMaterial().getTextureSettings0().enabled = true;
	skyboxRenderer->getMaterial().getTextureSettings0().cubemap = _skybox;
	skyboxRenderer->getMaterial().getRenderSettings().depthWrite = false;
	skyboxRenderer->getMaterial().setRenderQueue(ge::RQ_BACKGROUND);

	shared_ptr<ge::Mesh> worldBoxMesh = ge::pgeom::sphere(32, 32, 400, true);

	shared_ptr<ge::MeshR> worldBoxMeshRenderer(new ge::MeshR());
	worldBoxMeshRenderer->setMesh(worldBoxMesh);
	worldBoxMeshRenderer->setMaterial(ge::Material(ge::ShaderProgram::find("uni_inner")));
	worldBoxMeshRenderer->getMaterial().getRenderSettings().wireframe = true;

	btCollisionShape* collisionShape = builtBtBvhTriangleMeshShape(worldBoxMesh);

	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	
	vector<shared_ptr<ge::Renderable> > uni;
	uni.push_back(worldBoxMeshRenderer);
	uni.push_back(skyboxRenderer);
	artemis::Entity & universe = em->create();
	universe.addComponent(new RenderComponent(uni));
	universe.addComponent(new PhysicsComponent(collisionShape, groundRigidBody));// COLL_WORLD, COLL_BOX));
	universe.setGroup("WORLD");
	universe.refresh();

	collisionShape->setUserPointer(&universe);

	// For testing purposes.
	{
		std::vector<shared_ptr<ge::MeshR> > shape = resource::loadobj("teapot.obj", "teapot");
		artemis::Entity & entity = em->create();

		glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);

		for (auto p = shape.begin(); p != shape.end(); ++p) {
			ge::Transform t = (*p)->getTransform();
			(*p)->setTransform(t.setTranslation(t.getTranslation() + glm::vec3(10, 10, 10)));
			skyboxRenderer->getMaterial().getTextureSettings0().enabled = true;
			skyboxRenderer->getMaterial().getTextureSettings0().cubemap = _skybox;
		}

		entity.addComponent(new RenderComponent(shape));
		entity.refresh();
	}

	return universe;
}

artemis::Entity& EntityFactory::createPlayer(const glm::vec3& position) {
	std::vector<shared_ptr<ge::MeshR> > shape = resource::loadobj("player.nff", "diffuse");

	// XXX:
	glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
	glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	for (auto p = shape.begin(); p != shape.end(); ++p) {
		(*p)->getMaterial().setColor("lightDiffuse", lightDiffuse);
		(*p)->getMaterial().setColor("lightAmbient", lightAmbient);
	}

	btConvexHullShape* collisionShape = buildBtConvexHull(shape[0]->getMesh(), shape[0]->getTransform().getScale());

	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), fromGLM(position)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	collisionShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, collisionShape, fallInertia);
	btRigidBody* rigidBody = new btRigidBody(fallRigidBodyCI);

	// Don't permit the player object to deactivate.
	rigidBody->setActivationState(DISABLE_DEACTIVATION);

	artemis::EntityManager* em = _world->getEntityManager();

	artemis::Entity & player = em->create();
	player.addComponent(new PlayerComponent());
	// Collision flags.
	// COLL_BOX, COLL_WORLD));
	player.addComponent(new PhysicsComponent(collisionShape, rigidBody));
	player.addComponent(new TransformComponent(shape, position));
	player.addComponent(new RenderComponent(shape));
	player.refresh();
	
	return player;
}

// TODO:
namespace {
	unsigned seed = 1234;
	std::mt19937 random(seed);
}
artemis::Entity& EntityFactory::createAsteroid(const glm::vec3& position) {
	size_t asteroidIndex = static_cast<size_t>(((float)random() / random.max()) *_asteroids.size());
	size_t textureIndex = static_cast<size_t>(((float)random() / random.max()) * _textures.size());
	shared_ptr<ge::MeshR> meshR(new ge::MeshR(*_asteroids[asteroidIndex].get()));
	
	meshR->getMaterial().getTextureSettings0().enabled = true;
	meshR->getMaterial().getTextureSettings0().texture = _textures[textureIndex];

	// Set the lighting uniforms.
	meshR->getMaterial().setColor("matDiffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	glm::vec4 lightDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	meshR->getMaterial().setColor("lightDiffuse", lightDiffuse);
	meshR->getMaterial().setColor("lightAmbient", lightAmbient);

	meshR->setTransform(meshR->getTransform().setTranslation(position));

	btConvexHullShape* shape = buildBtConvexHull(meshR->getMesh(), meshR->getTransform().getScale());

	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), fromGLM(position)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	shape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, shape, fallInertia);
	btRigidBody* rigidBody = new btRigidBody(fallRigidBodyCI);
	float x = 1.0f - ((float)random() / random.max()) * 2.0f;
	float y = 1.0f - ((float)random() / random.max()) * 2.0f;
	float z = 1.0f - ((float)random() / random.max()) * 2.0f;
	size_t speed = 10.0f;
	rigidBody->applyCentralImpulse(btVector3(x * speed, y * speed, z * speed));

	artemis::EntityManager* em = _world->getEntityManager();
	artemis::Entity & asteroid = em->create();
	asteroid.addComponent(new PhysicsComponent(shape, rigidBody));// COLL_BOX, COLL_WORLD));
	asteroid.addComponent(new TransformComponent(meshR->getTransform()));
	asteroid.addComponent(new RenderComponent(meshR));
	asteroid.setGroup("asteroid");
	asteroid.refresh();

	return asteroid;
}