/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/PlayerSystem.h>
#include <asteroids/PhysicsSystem.h>
#include <asteroids/BulletUtil.h>
#include <asteroids/RenderSystem.h>

#include <SFML/Graphics.hpp>

#include <ge/Camera.h>

#include <artemis/World.h>
#include <artemis/SystemManager.h>

#include <btBulletDynamicsCommon.h>

using namespace asteroids;
using namespace std;

PlayerSystem::PlayerSystem(sf::Window* window) : _window(window), _reset(false) {
	setComponentTypes<PlayerComponent, TransformComponent>();
}

void PlayerSystem::initialize() {
	playerMapper.init(*world);
	transformMapper.init(*world);
	physicsMapper.init(*world);

	RenderSystem& renderSystem = (RenderSystem&)world->getSystemManager()->getSystem<RenderSystem>();
	_camera = renderSystem.getMainCamera();
}

void PlayerSystem::processEntity(artemis::Entity &e) {
	PlayerComponent* component = playerMapper.get(e);
	TransformComponent* transform = transformMapper.get(e);
	PhysicsComponent* physics = physicsMapper.get(e);

	if (_reset) {
		_reset = false;
		btTransform tr;
		tr.setIdentity();
		tr.setOrigin(btVector3(0, 0, 0));

		physics->rigid->activate(true);
		physics->rigid->setWorldTransform(tr);
		physics->rigid->setLinearVelocity(fromGLM(glm::vec3()));
		physics->rigid->setAngularVelocity(fromGLM(glm::vec3()));
	}
	else {
		glm::vec3 torque;

		// Rotation about X.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))  {
			torque = transform->transform.getRight();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))  {
			torque = -transform->transform.getRight();
		}
		// Rotation about Z.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))  {
			torque = transform->transform.getForward();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))  {
			torque = -transform->transform.getForward();
		}
		
		// Force and torque are in world space, not local.
		if (torque != glm::vec3()) {
			torque *= 4.0f;
			physics->rigid->applyTorque(btVector3(torque.x, torque.y, torque.z));
		}
		else {
			// TODO: Perhaps instead lower the angular velocity gradually?
			physics->rigid->setAngularVelocity(fromGLM(glm::vec3()));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			glm::vec3 force = transform->transform.getUp() * 10.0f;
			// 10 newtons of the force in the up direction.
			physics->rigid->applyCentralForce(fromGLM(force));
		}
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))  {
		physics->rigid->clearForces();
		physics->rigid->setLinearVelocity(fromGLM(glm::vec3()));
		physics->rigid->setAngularVelocity(fromGLM(glm::vec3()));
	}


	ge::Transform cameraTransform = _camera->getTransform();
	cameraTransform.setTranslation(transform->transform.getTranslation() - transform->transform.getUp() * 20.0f + transform->transform.getForward());
	cameraTransform.lookAt(transform->transform, transform->transform.getUp());

	// Prevent the objects coming between the camera and the ship.
	btCollisionWorld::ClosestRayResultCallback ray(fromGLM(cameraTransform.getTranslation()), fromGLM(transform->transform.getTranslation()));

	PhysicsSystem& physicsSystem = (PhysicsSystem&)world->getSystemManager()->getSystem<PhysicsSystem>();
	physicsSystem.getPhysicsWorld()->rayTest(fromGLM(cameraTransform.getTranslation()), fromGLM(transform->transform.getTranslation()), ray);
	if (ray.hasHit() && ray.m_collisionObject != physics->rigid) {
		cameraTransform.setTranslation(toGLM(ray.m_hitPointWorld));
	}
	_camera->setTransform(cameraTransform);
}
