/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <ge/Cubemap.h>

#include <SFML/Graphics.hpp>

#include <memory>

namespace ge {
class RenderContext;
class TextureSettings {
public:

	bool enabled;
	std::shared_ptr<sf::Texture> texture;
	std::shared_ptr<Cubemap> cubemap;

	TextureSettings();
	virtual ~TextureSettings();

	void bind(int unit, RenderContext& context);
};
}