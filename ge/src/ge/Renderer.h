/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/RenderQueue.h>
#include <ge/RenderContext.h>

#include <vector>
#include <map>
#include <memory>

namespace ge {
	// Render queue constants.

	const int RQ_BACKGROUND = 0;
	const int RQ_OPAQUE = 10;
	const int RQ_TRANSPARENT = 20;
	const int RQ_OVERLAY = 20;

class Rectangle2;
class Renderer {

	glm::vec4 _clearColor;
	int _clearStencilValue;

	std::map<int, std::unique_ptr<RenderQueue> > _rq;
	bool _processingQueue;

	// No need for a stack. This can be computed (projection * modelView)
	glm::mat4 _modelViewProjection;
	RenderContext _context;

public:

	Renderer();
	virtual ~Renderer();
	enum BUFFER {
		BUFFER_NONE = 0x00,
		BUFFER_COLOR = 0x01,
		BUFFER_DEPTH = 0x02,
		// OpenGL ES doesn't support accumulation buffer
		// BUFFER_ACCUMULATION = 0x04,
		BUFFER_STENCIL = 0x08,
		BUFFER_COLOR_AND_DEPTH = BUFFER_COLOR | BUFFER_DEPTH,
	};

	RenderContext& getContext() { return _context;  }

	void setBackgroundColor(const glm::vec4& c);
	const glm::vec4& getBackgroundColor() const;

	void setStencilClearValue(int value);

	int getStencilClearValue() const;

	void endFrame();

	void clearBuffers(int buffers);
	void clearBuffers(int buffers, bool strict);

	void processQueue();
	void processQueue(bool doSort, bool doClear);
	void clearQueue();

	bool isProcessingQueue() const {
		return _processingQueue;
	}
	void checkAndAdd(Renderable* s);

	void checkCardError();

	bool doTransforms(const glm::mat4& worldMatrix);
	void undoTransforms();

	void setViewport(int x, int y, int width, int height);

	void setDepthRange(float depthRangeNear, float depthRangeFar);

	void pushClip(const Rectangle2& rectangle);

	void pushEmptyClip();
	void popClip();
	void clearClips();
	void setClipTestEnabled(bool enabled);
	bool isClipTestEnabled();
};
}