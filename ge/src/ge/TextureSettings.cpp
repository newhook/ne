/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/TextureSettings.h>
#include <ge/RenderContext.h>

using namespace ge;

TextureSettings::TextureSettings() : enabled(false) {

}

TextureSettings::~TextureSettings() {

}

void
TextureSettings::bind(int unit, RenderContext& context) {
	if (!enabled) {
		return;
	}
	if (context.textureUnit != unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		context.textureUnit = unit;
	}

	if (texture) {
		sf::Texture::bind(texture.get(), sf::Texture::CoordinateType::Normalized);
	}

	if (cubemap) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getTextureId());
	}
}
