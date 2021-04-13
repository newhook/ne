/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <ge/Animation.h>
#include <ge/SkinMeshR.h>

namespace wc {

	class AnimationComponent : public artemis::Component
	{
	public:
		std::shared_ptr<ge::Animator> animator;
		std::shared_ptr<ge::SkinMeshR> skinnedMeshR;
		AnimationComponent(const std::shared_ptr<ge::Animator>& animator, const std::shared_ptr<ge::SkinMeshR>& skinnedMeshR)
		{
			this->skinnedMeshR = skinnedMeshR;
			this->animator = animator;
		}
	};
}