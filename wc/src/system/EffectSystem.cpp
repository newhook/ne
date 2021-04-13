/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <system/EffectSystem.h>
#include <system/RenderSystem.h>

#include <EntityFactory.h>

#include <glm/gtx/transform.hpp>

#include <ge/Logger.h>

using namespace wc;
using namespace std;

namespace {
	ge::Logger logger(__FILE__);
}
EffectSystem::EffectSystem() : EntityProcessingSystem(artemis::Aspect::getAspectForOne<ScaleComponent, FlipbookComponent, BillboardComponent>()) {
}

void EffectSystem::initialize() {
}


void EffectSystem::processEntity(const std::shared_ptr<artemis::Entity>& e) {
	RenderComponent* render = renderMapper.get(e);

	ScaleComponent* scale = scaleMapper.get(e);
	TransformComponent* transform = transformMapper.get(e);
	if (scale) {
		scale->time += world->getDelta();
		float s;
		if (scale->time > scale->duration) {
			s = scale->end;
		}
		else {
			s = glm::mix(scale->start, scale->end, scale->time / scale->duration);
		}

		transform->transform.setScale(glm::vec3(s, s, s));
	}

	FlipbookComponent* fb = fbMapper.get(e);
	if (fb) {
		fb->time += world->getDelta();
		int widthSq = fb->width * fb->width;
		float t = (fb->time / fb->period) * widthSq;
		int frame = glm::floor(t);

		float ratio = t - frame;

		int row = frame / fb->width;
		int col = frame % fb->width;
		glm::vec2 f0 = glm::vec2(col * 1.0 / fb->width, row * 1.0 / fb->width);

		// Next frame.
		frame++;
		row = frame / fb->width;
		col = frame % fb->width;
		glm::vec2 f1 = glm::vec2(col * 1.0 / fb->width, row * 1.0 / fb->width);

		if (frame > widthSq) {
			ratio = 0.0;
		}

		for (auto p : render->renderable) {
			p->getMaterial().setVector("f0", f0);
			p->getMaterial().setVector("f1", f1);
			p->getMaterial().setFloat("ratio", ratio);
			p->getMaterial().setFloat("width", fb->width);
		}
	}

	BillboardComponent* bb = bbMapper.get(e);
	if (bb) {
		RenderSystem* renderSystem = world->getSystem<RenderSystem>();
		glm::vec3 look = glm::normalize(renderSystem->getMainCamera()->getTransform().getTranslation() - transform->transform.getTranslation());
		glm::vec3 right = glm::cross(renderSystem->getMainCamera()->getTransform().getUp(), look);
		glm::vec3 up = glm::cross(look, right);

		glm::mat4 mat;
		mat[0] = glm::vec4(right, 0);
		mat[1] = glm::vec4(up, 0);
		mat[2] = glm::vec4(look, 0);

		transform->transform.setRotation(glm::quat(mat) * bb->rot);	
	}
}
