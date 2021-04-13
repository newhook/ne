/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <Config.h>
#include <EntityFactory.h>
#include <component/PhysicsComponent.h>
#include <component/RenderComponent.h>
#include <component/TransformComponent.h>
#include <component/TerrainComponent.h>
#include <component/PlayerComponent.h>
#include <component/TowerComponent.h>
#include <component/TargetComponent.h>
#include <component/CreepComponent.h>
#include <component/PathComponent.h>
#include <component/ObstructionComponent.h>
#include <component/ProjectileComponent.h>
#include <component/ExpireComponent.h>
#include <component/ScaleComponent.h>
#include <component/AnimationComponent.h>
//#include <CollisionSystem.h>
#include <bullet/BulletUtil.h>
#include <system/RenderSystem.h>
#include <Resource.h>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <ge/Renderer.h>
#include <ge/Mesh.h>
#include <ge/ShaderProgram.h>
#include <ge/PGeom.h>
#include <ge/MeshR.h>
#include <ge/Material.h>

#include <ge/InputStream.h>

#include <sstream>
#include <random>

using namespace wc;
using namespace std;

namespace {
	shared_ptr<sf::Texture> _terrainTexture;
	shared_ptr<sf::Image> _heightfield;	
	shared_ptr<ge::Cubemap> _skybox;

	shared_ptr<ge::Mesh> explosionMesh;
	shared_ptr<sf::Texture> explosionTexture;
	shared_ptr<ge::Mesh> projectileMesh;
}

EntityFactory::EntityFactory() {
	//_heightfield = resource::loadImage("models/heightmap.bmp");
	_heightfield = make_unique<sf::Image>();
	//_heightfield->create(256, 256, sf::Color::Black);
	_heightfield->create(64, 64, sf::Color::Black);

	///_heightfield = shared_ptr<sf::Image>(new sf::Image());
	//_heightfield->create(32, 32);
	sf::Image img;
	img.create(1024, 1024, sf::Color::Green);
	//_terrainTexture = resource::loadTexture("models/terrain.png");
	_terrainTexture = make_unique<sf::Texture>();
	_terrainTexture->loadFromImage(img);

	explosionTexture = resource::loadTexture("models/explosion3.png");
	explosionMesh = ge::pgeom::quad(4.0, 4.0);//ge::pgeom::sphere(8, 8, 1.0f);
	projectileMesh = ge::pgeom::box(glm::vec3(), glm::vec3(0.5f));
}

void
EntityFactory::initialize() {
}


// Create the world box.
std::shared_ptr<artemis::Entity> EntityFactory::createUniverse()
{
	RenderSystem* renderSystem = _world->getSystem<RenderSystem>();

	unique_ptr<TerrainComponent> terrainComponent = make_unique<TerrainComponent>(_heightfield);
	terrainComponent->translate = false;

	sf::Image hf(*_heightfield);
	hf.flipVertically();
	shared_ptr<sf::Texture> tex(new sf::Texture());
	tex->loadFromImage(hf);

	int width = _heightfield->getSize().x;
	int height = _heightfield->getSize().y;

	float horizontalExtend = static_cast<float>(width);
	float verticalExtend = static_cast<float>(height);

	shared_ptr<ge::Mesh> mesh(ge::pgeom::gridPlane(horizontalExtend, verticalExtend, width, height));

	// Setup the heights.
	vector<glm::vec3>& vertices = mesh->vertices;
	for (size_t i = 0; i < vertices.size(); ++i) {
		vertices[i].z = vertices[i].y;
		vertices[i].y = 0;
		vertices[i].y = terrainComponent->getHeight(vertices[i]);
	}

	// Setup the normals.
	vector<glm::vec3>& normals = mesh->normals;
	for (size_t i = 0; i < normals.size(); ++i) {
		glm::vec4 h;
		h[0] = terrainComponent->getHeight(vertices[i] - glm::vec3(1, 0, 0));
		h[1] = terrainComponent->getHeight(vertices[i] - glm::vec3(0, 0, 1));
		h[2] = terrainComponent->getHeight(vertices[i] + glm::vec3(1, 0, 0));
		h[3] = terrainComponent->getHeight(vertices[i] + glm::vec3(0, 0, 1));

		glm::vec3& n = normals[i];
		n.z = h[0] - h[3];
		n.x = h[1] - h[2];
		n.y = 2.0;
		n = glm::normalize(n);
	}

	shared_ptr<ge::MeshR> meshR(make_shared<ge::MeshR>());

	meshR->setMesh(mesh);
	meshR->setMaterial(ge::Material(ge::ShaderProgram::find("lambert")));

	// Lighting uniforms.
	meshR->getMaterial().setColor("m_diffuse", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	meshR->getMaterial().setColor("m_ambient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	meshR->getMaterial().getTextureSettings0().enabled = true;
	meshR->getMaterial().getTextureSettings0().texture = _terrainTexture;
	meshR->getMaterial().getRenderSettings().cullMode = ge::CullMode::None;

	vector<shared_ptr<ge::MeshR> > terrainMeshR;
	terrainMeshR.push_back(meshR);

	// It would be better to use the heightfield. Figure it out!
#ifdef never
	float* data = new float[_heightfield->getSize().x * _heightfield->getSize().y];
	float* dataCurr = data;
	const float* dataEnd = data + _heightfield->getSize().x * _heightfield->getSize().y;
	const sf::Uint8* pixels = _heightfield->getPixelsPtr();
	while (dataCurr < dataEnd) {
		*dataCurr++ = (*pixels / 255.0f) * terrainComponent->bias;
		pixels += 3;
	}

	// TODO: free data when the component is destroyed.
	btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(_heightfield->getSize().x, _heightfield->getSize().y, data, 
		1.0f /*unused with float*/, 0, terrainComponent->bias, 1, PHY_FLOAT, true);
#endif

	unique_ptr<btCollisionShape> collisionShape = builtBtBvhTriangleMeshShape(mesh);
	// For the world box.
	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, motionState, collisionShape.get(), btVector3(0, 0, 0));
	std::unique_ptr<btRigidBody> groundRigidBody = make_unique<btRigidBody>(groundRigidBodyCI);

	vector<shared_ptr<ge::Renderable> > uni(terrainMeshR.begin(), terrainMeshR.end());
	std::shared_ptr<artemis::Entity> universe = _world->createEntity();
	universe->addComponent(make_unique<RenderComponent>(uni));
	universe->addComponent(std::move(terrainComponent));
	universe->addComponent(make_unique<TransformComponent>(glm::vec3()));
	universe->addComponent(make_unique<PhysicsComponent>(std::move(collisionShape), std::move(groundRigidBody)));// COLL_WORLD, COLL_BOX));
	_world->getManager<artemis::TagManager>()->_register("terrain", universe);

	//collisionShape->setUserPointer(&universe);

	return universe;
}

std::shared_ptr<artemis::Entity> EntityFactory::createPlayer(const glm::vec3& position1, int team) {
	std::shared_ptr<artemis::Entity> terrain = _world->getManager<artemis::TagManager>()->getEntity("terrain");
	TerrainComponent* terrainComponent = terrain->getComponent<TerrainComponent>();
	glm::vec3 position = position1;
	position.y = terrainComponent->getHeight(position) + 1.1f;

	std::shared_ptr<resource::Resource> resource2 = resource::loadobj("Animations/knight@running.fbx");
	std::shared_ptr<resource::Resource> resource = resource::loadobj("Enemies/knight.fbx");
	vector<shared_ptr<ge::SkinMeshR> > shape = resource->getSkinMeshR();

	std::shared_ptr<ge::SkeletonPose> pose = make_shared<ge::SkeletonPose>(resource->skeleton);
	std::shared_ptr<ge::Animator> animator = make_shared<ge::Animator>(pose);
	animator->addClip("running", resource2->animation);
	animator->play("running");


	/*
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
	*/

	std::shared_ptr<artemis::Entity> entity = _world->createEntity();
	entity->addComponent(make_unique<PlayerComponent>());
	entity->addComponent(make_unique<PathComponent>());
	PathComponent* path = entity->getComponent<PathComponent>();
	// TODO: Table
	path->speed = 5.0f;

	// Collision flags.
	// COLL_BOX, COLL_WORLD));
	//player.addComponent(new PhysicsComponent(collisionShape, rigidBody));
	entity->addComponent(make_unique<AnimationComponent>(animator, *shape.begin()));
	entity->addComponent(make_unique<TransformComponent>(shape, position));
	entity->addComponent(make_unique<ObstructionComponent>());
	entity->addComponent(make_unique<RenderComponent>(shape));
	entity->addComponent(make_unique<TargetComponent>(team));
	
	_world->getManager<artemis::GroupManager>()->add(entity, "target");

	return entity;
}

std::shared_ptr<artemis::Entity> EntityFactory::createCreep(const glm::vec3& position1, int team) {
	std::shared_ptr<artemis::Entity> terrain = _world->getManager<artemis::TagManager>()->getEntity("terrain");
	TerrainComponent* terrainComponent = terrain->getComponent<TerrainComponent>();
	glm::vec3 position = position1;
	position.y = terrainComponent->getHeight(position) + 0.1f;

	std::shared_ptr<resource::Resource> resource = resource::loadobj("Enemies/knight2.fbx");

	vector<shared_ptr<ge::MeshR> > shape = resource->getMeshR();

	// XXX:
	glm::vec4 lightDiffuse = glm::vec4(1.0f);
	glm::vec4 lightAmbient = glm::vec4(1.0f);

	for (auto p = shape.begin(); p != shape.end(); ++p) {
		(*p)->getMaterial().setColor("l_diffuse", lightDiffuse);
		(*p)->getMaterial().setColor("l_ambient", lightAmbient);
	}

	/*
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
	*/
	std::shared_ptr<artemis::Entity> entity = _world->createEntity();
	entity->addComponent(make_unique<CreepComponent>());
	
	//entity->addComponent(new PhysicsComponent(collisionShape, rigidBody));
	entity->addComponent(make_unique<ObstructionComponent>());
	entity->addComponent(make_unique<TargetComponent>(team));
	entity->addComponent(make_unique<PathComponent>());
	entity->addComponent(make_unique<TransformComponent>(shape, position));
	entity->addComponent(make_unique<RenderComponent>(shape));
	_world->getManager<artemis::GroupManager>()->add(entity, "target");

	return entity;
}

std::shared_ptr<artemis::Entity> EntityFactory::createTower(const glm::vec3& position1, int team) {
	std::shared_ptr<artemis::Entity> terrain = _world->getManager<artemis::TagManager>()->getEntity("terrain");
	TerrainComponent* terrainComponent = terrain->getComponent<TerrainComponent>();
	glm::vec3 position = position1;
	position.y = terrainComponent->getHeight(position) + 0.1f;

	std::shared_ptr<resource::Resource> resource = resource::loadobj("Tower/Cannontower.fbx");
	vector<shared_ptr<ge::MeshR> > shape = resource->getMeshR();
	for (auto p = shape.begin(); p != shape.end(); ++p) {
		(*p)->getMaterial().getRenderSettings().cullMode = ge::CullMode::None;
	}

	/*
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
	*/

	std::shared_ptr<artemis::Entity> entity = _world->createEntity();
	entity->addComponent(make_unique<TowerComponent>());
	entity->addComponent(make_unique<TargetComponent>(team));
	//towerEntity.addComponent(new PhysicsComponent(collisionShape, rigidBody));
	entity->addComponent(make_unique<TransformComponent>(shape, position));
	entity->addComponent(make_unique<RenderComponent>(shape));
	entity->addComponent(make_unique<ObstructionComponent>());

	return entity;
}

std::shared_ptr<artemis::Entity>
EntityFactory::createShot(const std::shared_ptr<artemis::Entity>& source, const std::shared_ptr<artemis::Entity>& target) {
	TransformComponent* targetTransform = target->getComponent<TransformComponent>();
	TransformComponent* sourceTransform = source->getComponent<TransformComponent>();

	shared_ptr<ge::MeshR> meshR(make_shared<ge::MeshR>());
	meshR->setMesh(projectileMesh);
	meshR->setMaterial(ge::Material(ge::ShaderProgram::find("lambert")));

	std::shared_ptr<artemis::Entity> entity = _world->createEntity();
	entity->addComponent(make_unique<TransformComponent>(ge::Transform()));
	entity->addComponent(make_unique<RenderComponent>(meshR));
	entity->addComponent(make_unique<ProjectileComponent>(sourceTransform->getWorldTransform().getTranslation(), targetTransform->getWorldTransform().getTranslation()));

	return entity;
}

std::shared_ptr<artemis::Entity> EntityFactory::createExplosion(const glm::vec3& position) {
	shared_ptr<ge::MeshR> meshR(make_shared<ge::MeshR>());
	meshR->setMesh(explosionMesh);
	meshR->setMaterial(ge::Material(ge::ShaderProgram::find("flipbook")));

	meshR->getMaterial().setRenderQueue(ge::RQ_TRANSPARENT + 1);
	meshR->getMaterial().getRenderSettings().blendEnabled = true;
	meshR->getMaterial().getRenderSettings().sourceBlendFunctionAlpha = ge::SourceFunction::OneMinusSourceAlpha;
	meshR->getMaterial().getTextureSettings0().enabled = true;
	meshR->getMaterial().getTextureSettings0().texture = explosionTexture;

	std::shared_ptr<artemis::Entity> entity = _world->createEntity();
	ge::Transform transform;
	transform.setTranslation(position);
	entity->addComponent(make_unique<TransformComponent>(transform));
	entity->addComponent(make_unique<RenderComponent>(meshR));
	entity->addComponent(make_unique<ExpireComponent>(1.0f));
	entity->addComponent(make_unique<FlipbookComponent>(4, 1.0f));
	entity->addComponent(make_unique<BillboardComponent>(glm::quat()));

	return entity;
}

std::shared_ptr<artemis::Entity> EntityFactory::createPick(const glm::vec3& position) {
	shared_ptr<ge::MeshR> meshR(make_shared<ge::MeshR>());
	meshR->setMesh(explosionMesh);
	meshR->setMaterial(ge::Material(ge::ShaderProgram::find("lambert")));

	std::shared_ptr<artemis::Entity> entity = _world->createEntity();
	ge::Transform transform;
	transform.setTranslation(position);
	entity->addComponent(make_unique<TransformComponent>(transform));
	entity->addComponent(make_unique<RenderComponent>(meshR));
	entity->addComponent(make_unique<ExpireComponent>(1.0f));
	entity->addComponent(make_unique<ScaleComponent>(0.1f, 1.0f, 1.0f));

	return entity;
}