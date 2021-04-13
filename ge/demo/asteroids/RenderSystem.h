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

#include <asteroids/RenderComponent.h>
#include <asteroids/TransformComponent.h>

#include <ge/Renderable.h>
#include <ge/Lines.h>
#include <ge/Camera.h>

#include <memory>

namespace asteroids {
	class BulletDebugDrawer;
	class RenderSystem : public artemis::EntityProcessingSystem
	{
		ge::Renderer* _renderer;

		ge::Camera _orthoCamera;
		ge::Camera _mainCamera;
		ge::Camera _skyboxCamera;

		artemis::ComponentMapper<RenderComponent> renderMapper;
		artemis::ComponentMapper<TransformComponent> transformMapper;

		std::vector<std::shared_ptr<ge::MeshR> > _axis;
		std::vector<ge::Transform> _axisLocal;

		bool _changeWireframe;
		bool _wireframe;
		bool _showBounds;
		bool _showScene;

		glm::vec4 _lightDir;

		std::shared_ptr<ge::Lines> _lines;

		std::vector<std::shared_ptr<ge::Renderable> > _renderables;

		float _time;

	public:

		RenderSystem(ge::Renderer* renderer, int width, int height);

		virtual void initialize();

		bool getWireframe() const {
			return _wireframe;
		}
		void setWireframe(bool v) {
			_changeWireframe = true;
			_wireframe = v;
		}

		bool getShowBounds() const {
			return _showBounds;
		}
		void setShowBounds(bool v) {
			_showBounds = v;
		}

		bool getShowScene() const {
			return _showScene;
		}
		void setShowScene(bool v) {
			_showScene = v;
		}

		virtual void processEntity(artemis::Entity &e);

		virtual void begin();

		virtual void end();

		virtual void added(artemis::Entity &e);

		void drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& fromColor, const glm::vec4& toColor);

		ge::Camera* getOrthoCamera() {
			return &_orthoCamera;
		}

		ge::Camera* getMainCamera() {
			return &_mainCamera;
		}

		ge::Camera* getSkyboxCamera() {
			return &_skyboxCamera;
		}
	};

}