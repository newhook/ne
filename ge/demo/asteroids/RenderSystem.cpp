/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/RenderSystem.h>
#include <asteroids/Resource.h>

#include <ge/Renderer.h>
#include <ge/Camera.h>
#include <ge/Mesh.h>
#include <ge/MeshR.h>
#include <ge/LinesR.h>
#include <ge/PGeom.h>
#include <ge/ShaderProgram.h>

using namespace asteroids;
using namespace std;

RenderSystem::RenderSystem(ge::Renderer* renderer, int width, int height) : _renderer(renderer), _changeWireframe(false), _wireframe(false), _showBounds(false), _showScene(true), _time(0.0f) {

	_orthoCamera.setMode(ge::Camera::ORTHO);
	_orthoCamera.setViewport(0, 0, width, height);

	_mainCamera.setFOV(45.0f);
	_mainCamera.setViewport(0, 0, width, height);
	//_camera.setClipping(1, 1000);
	_mainCamera.setClipping(1, 5000);

	_mainCamera.setTransform(_mainCamera.getTransform().setTranslation(glm::vec3(0.0f, 50.0f, 20.0f)).lookAt(glm::vec3(0.0f, 0.0f, 0.0f)));

	_skyboxCamera.setFOV(45.0f);
	_skyboxCamera.setClipping(1, 1000);
	_skyboxCamera.setViewport(0, 0, width, height);

	setComponentTypes<RenderComponent>();
}

void RenderSystem::initialize() {
	renderMapper.init(*world);
	transformMapper.init(*world);


	_axis = resource::loadobj("axis.nff", "axis");
	for (auto p = _axis.begin(); p != _axis.end(); ++p) {
		(*p)->setCamera(&_mainCamera);
		_axisLocal.push_back((*p)->getTransform());
	}

}

void RenderSystem::processEntity(artemis::Entity &e) {

	RenderComponent* renderComponent = renderMapper.get(e);
	TransformComponent* transformComponent = transformMapper.get(e);

	for (size_t i = 0; i < renderComponent->renderable.size(); ++i) {
		const shared_ptr<ge::Renderable>& renderable = renderComponent->renderable[i];

		// Update the renderable position if there is a transform component.
		if (transformComponent) {
			renderable->setTransform(transformComponent->getWorldTransform(i));
		}

		// TODO: Inefficient. Probably needs a ShaderComponent or similar.
		renderable->getMaterial().setFloat("time", _time);

		if (_changeWireframe) {
			renderable->getMaterial().getRenderSettings().wireframe = _wireframe;
		}

		if (_showBounds) {
			ge::Bounds b = renderable->getBounds();
			shared_ptr<ge::Mesh> bb = ge::pgeom::box(b.getCenter(), b.getExtents());
			bb->normals.clear();
			bb->uv.clear();

			shared_ptr<ge::MeshR> bmr(new ge::MeshR());
			bmr->setMesh(bb);
			bmr->setCamera(&_mainCamera);
			bmr->setMaterial(ge::Material(ge::ShaderProgram::find("debug")));
			bmr->getMaterial().getRenderSettings().wireframe = true;
			bmr->getMaterial().getRenderSettings().depthTest = true;
			bmr->getMaterial().getRenderSettings().depthWrite = true;
			bmr->getMaterial().setRenderQueue(ge::RQ_OPAQUE + 1);

			_renderables.push_back(bmr);
		}

		renderable->getMaterial().setColor("lightDirection", _lightDir);

		if (_showScene) {
			renderable->draw(_renderer);
		}
	}
}

void RenderSystem::begin() {
	_time += world->getDelta();

	_skyboxCamera.setTransform(_mainCamera.getTransform().setTranslation(glm::vec3()));

	_lightDir = _mainCamera.getModelViewMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

	_orthoCamera.update();
	_mainCamera.update();
	_skyboxCamera.update();

	_renderables.clear();
}

void RenderSystem::end() {
	if (_lines) {
		std::shared_ptr<ge::LinesR> linesR(new ge::LinesR());
		linesR->setLines(_lines);
		linesR->setCamera(&_mainCamera);
		linesR->setMaterial(ge::Material(ge::ShaderProgram::find("bullet")));
		_renderables.push_back(linesR);
	}

	_changeWireframe = false;
	for (auto p = _renderables.begin(); p != _renderables.end(); ++p) {
		(*p)->draw(_renderer);
	}

	{
		// Reposition the axis to the lower left corner.
		glm::vec4 pos = _mainCamera.getModelViewProjectionInverseMatrix() * glm::vec4(-0.9f, -0.9f, 0.0f, 1.0f);
		pos.w = 1.0f / pos.w;
		ge::Transform transform;
		transform.setScale(glm::vec3(0.1f));
		transform.setTranslation(glm::vec3(pos) * pos.w);

		// Apply the transform and render the axis.
		size_t i = 0;
		for (auto p = _axis.begin(); p != _axis.end(); ++p) {
			ge::Transform t = _axisLocal[i++];
			(*p)->setTransform(t.combineWithParent(transform));
			(*p)->draw(_renderer);
		}
	}

	_renderer->processQueue();

	if (_lines) {
		_lines->clear();
	}
}

void RenderSystem::added(artemis::Entity &e) {
	RenderComponent* renderComponent = renderMapper.get(e);
	if (renderComponent) {
		for (auto p = renderComponent->renderable.begin(); p != renderComponent->renderable.end(); ++p) {
			if (!(*p)->getCamera()) {
				(*p)->setCamera(&_mainCamera);
			}
		}
	}
}
void RenderSystem::drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& fromColor, const glm::vec4& toColor) {
	if (!_lines) {
		_lines = std::shared_ptr<ge::Lines>(new ge::Lines());
	}
	_lines->vertices.push_back(from);
	_lines->colors.push_back(fromColor);
	_lines->vertices.push_back(to);
	_lines->colors.push_back(toColor);
}