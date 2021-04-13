/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/PlayerSystem.h>
#include <system/PhysicsSystem.h>
#include <system/RenderSystem.h>
#include <component/TerrainComponent.h>
#include <bullet/BulletUtil.h>

#include <EntityFactory.h>

#include <SFML/Graphics.hpp>

#include <ge/Camera.h>

#include <btBulletDynamicsCommon.h>

using namespace wc;
using namespace std;

PlayerSystem::PlayerSystem(sf::Window* window) : EntityProcessingSystem(artemis::Aspect::getAspectFor<PlayerComponent, TransformComponent>()), _window(window), _reset(true), _terrain(0) {
}

void PlayerSystem::initialize() {
	RenderSystem* renderSystem = world->getSystem<RenderSystem>();
	_camera = renderSystem->getMainCamera();
}

void PlayerSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {

	// Dislike!
	_terrain = world->getManager<artemis::TagManager>()->getEntity("terrain");

	PathComponent* path = pathMapper.get(e);
	PlayerComponent* player = playerMapper.get(e);
	TransformComponent* transform = transformMapper.get(e);
	PhysicsComponent* physics = physicsMapper.get(e);
	
	bool reset = _reset;
	if (_reset) {
		_reset = false;

		//transform->transform.setTranslation(glm::vec3());

		//btTransform tr;
		//tr.setIdentity();
		//tr.setOrigin(btVector3(10.0f, 10.0f, 10.0f));

		//if (physics) {
		//	physics->rigid->activate(true);
		//	physics->rigid->setWorldTransform(tr);
		//	physics->rigid->setLinearVelocity(fromGLM(glm::vec3()));
		//	physics->rigid->setAngularVelocity(fromGLM(glm::vec3()));
		//}
	}
	//btVector3 velocity = physics->rigid->getLinearVelocity();
	//if (velocity.length() == 0) {
	//	physics->rigid->
	//}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		if (!player->rightDown) {
			player->rightDown = true;
			int x = sf::Mouse::getPosition(*_window).x;
			int y = sf::Mouse::getPosition(*_window).y;

			btVector3 rayFrom = fromGLM(_camera->getTransform().getTranslation());
			btVector3 rayTo = fromGLM(_camera->getTransform().getTranslation() + _camera->screenPointToRay(x, y) * 100.0f);
			btCollisionWorld::ClosestRayResultCallback ray(rayFrom, rayTo);

			// TODO: This should only look at terrain.
			PhysicsSystem* physicsSystem = world->getSystem<PhysicsSystem>();
			physicsSystem->getPhysicsWorld()->rayTest(rayFrom, rayTo, ray);
			if (ray.hasHit()) {
				glm::vec3 pick = toGLM(ray.m_hitPointWorld);
				//shared_ptr<artemis::Entity> pickEntity = world->getManager<EntityFactory>()->createPick(pick);
				shared_ptr<artemis::Entity> pickEntity = world->getManager<EntityFactory>()->createExplosion(pick + glm::vec3(0,1,0));
				pickEntity->addToWorld();
				path->setGoal(pick);
			}
		}
	}
	else {
		player->rightDown = false;
	}

	//if (dir != glm::vec3() || reset) {
	//	TerrainComponent* terrainComponent = _terrain->getComponent<TerrainComponent>();
	//	glm::vec3 position = transform->transform.getTranslation() + dir;
	//	if (position.x < -terrainComponent->extends.x) {
	//		position.x = -terrainComponent->extends.x;
	//	}
	//	else if (position.x > terrainComponent->extends.x) {
	//		position.x = terrainComponent->extends.x;
	//	}
	//	if (position.z < -terrainComponent->extends.z) {
	//		position.z = -terrainComponent->extends.z;
	//	}
	//	else if (position.z > terrainComponent->extends.z) {
	//		position.z = terrainComponent->extends.z;
	//	}
	//	position.y = terrainComponent->getHeight(position);

	//	printf("%.2f, %.2f, %.2f\n", position.x, position.y, position.z);
	//	transform->transform.setTranslation(position);

	//	TransformComponent* terrainTransform = _terrain->getComponent<TransformComponent>();
	//	if (terrainComponent->translate) {
	//		glm::vec3 tp = position;
	//		tp -= glm::vec3(0.0f, position.y, 0.0f);
	//		terrainTransform->transform.setTranslation(tp);
	//	}

	//	RenderComponent* renderComponent = _terrain->getComponent<RenderComponent>();
	//	if (renderComponent) {
	//		ge::Material& m = renderComponent->renderable[0]->getMaterial();
	//		m.setVector("center", position);
	//	}
	//}
	
	// Follow camera.
	glm::vec3 cameraOffset(0.0f, 20.0f, 30.0f);
	//glm::vec3 cameraOffset(0.0f, 120.0f, 40.0f);
	ge::Transform cameraTransform = _camera->getTransform();
	cameraTransform.setTranslation(transform->transform.getTranslation() + cameraOffset);
	cameraTransform.lookAt(transform->transform, transform->transform.getUp());

	_camera->setTransform(cameraTransform);
}