/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <Config.h>

#include <ge/MeshR.h>
#include <ge/SkinMeshR.h>
#include <ge/Animation.h>

#include <vector>
#include <memory>

namespace wc {
	namespace resource {
		class Resource {
		public:

			std::vector<std::shared_ptr<ge::MeshR> > meshR;
			std::vector<std::shared_ptr<ge::SkinMeshR> > skinMeshR;

			std::shared_ptr<ge::AnimationClip> animation;
			std::shared_ptr<ge::Skeleton> skeleton;

			std::vector<std::shared_ptr<ge::MeshR> > getMeshR() const;
			std::vector<std::shared_ptr<ge::SkinMeshR> > getSkinMeshR() const;
		};
		std::shared_ptr<Resource> loadobj(const std::string& name, const std::string& shader = "");
		
		std::shared_ptr<sf::Texture> whiteTexture();
		void addTextureLocation(const std::string& path);
		std::shared_ptr<sf::Texture> loadTexture(const std::string& name);
		std::shared_ptr<sf::Image> loadImage(const std::string& name);
	}
}