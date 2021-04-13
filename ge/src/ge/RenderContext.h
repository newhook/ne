/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <ge/RenderSettings.h>
#include <ge/Rectangle2.h>

#include <vector>
namespace ge {
class RenderContext {
	

public:

	RenderContext();
	virtual ~RenderContext();

	// The current active texture unit.
	int textureUnit;

	// The current active shader.
	int shaderId;

	// Bound vao.
	int vao;

	bool clippingTestValid;
	bool clippingTestEnabled;
	std::vector<Rectangle2> clips;
	Rectangle2 scissor;	
	
	float depthRangeNear;
	float depthRangeFar;
	Rectangle2 viewport;
	
	RenderSettings renderSettings;
};
}