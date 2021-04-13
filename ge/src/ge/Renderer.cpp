/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Renderer.h>
#include <ge/Camera.h>
#include <ge/Material.h>
#include <ge/Renderable.h>
#include <ge/Exception.h>

using namespace ge;
using namespace std;

namespace {
//	b3d::Logger logger("com.bittrap.b3d.Renderer", __FILE__);

void
applyScissors(RenderContext* context) {
	const std::vector<Rectangle2>& clips = context->clips;

	if (!clips.empty()) {
		Rectangle2 init = clips.back();
		if (context->scissor != init) {
			glScissor(init.getX(), init.getY(), init.getWidth(), init.getHeight());
			context->scissor = init;
		}
	}
	else {
		Rectangle2 vp = context->viewport;
		if (context->scissor != vp) {
			glScissor(vp.getX(), vp.getY(), vp.getWidth(), vp.getHeight());
			context->scissor = vp;
		}
	}
}

void
setClippingEnabled(RenderContext* context, bool enabled) {
	if (enabled && (!context->clippingTestValid || !context->clippingTestEnabled)) {
		glEnable(GL_SCISSOR_TEST);
		context->clippingTestEnabled = true;
	}
	else if (!enabled && (!context->clippingTestValid || context->clippingTestEnabled)) {
		glDisable(GL_SCISSOR_TEST);
		context->clippingTestEnabled = false;
	}
	context->clippingTestValid = true;
}
}

// Class

Renderer::Renderer() :
	_clearColor(0.0f, 0.0f, 0.0f, 0.0f),
	_clearStencilValue(0),
	_processingQueue(false) {
	// Initialize the render settings to the gl defaults.
	RenderSettings& settings = _context.renderSettings;
	settings.first = true;
	settings.wireframe = false;
	settings.cullMode = CullMode::Back;
	settings.depthTest = false;
	settings.polygonWind = PolygonWind::CounterClockWise;
	settings.depthFunction = TestFunction::EqualTo;
	settings.depthWrite = true;
	settings.colorWriteRed = true;
	settings.colorWriteGreen = true;
	settings.colorWriteBlue = true;
	settings.colorWriteAlpha = true;
	settings.blendEnabled = false;
	settings.blendColor = glm::vec4(0, 0, 0, 0);
	settings.sourceBlendFunctionRGB = SourceFunction::One;
	settings.destinationBlendFunctionRGB = DestinationFunction::Zero;
	settings.sourceBlendFunctionAlpha = SourceFunction::One;
	settings.destinationBlendFunctionAlpha = DestinationFunction::Zero;
	settings.blendEquationRGB = BlendEquation::Add;
	settings.blendEquationAlpha = BlendEquation::Add;
}

Renderer::~Renderer() {
}

void
Renderer::setBackgroundColor(const glm::vec4& color) {
	_clearColor = color;
	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
}

const glm::vec4&
Renderer::getBackgroundColor() const {
	return _clearColor;
}

void
Renderer::setStencilClearValue(int value) {
	_clearStencilValue = value;
	glClearStencil(_clearStencilValue);
}

int
Renderer::getStencilClearValue() const {
	return _clearStencilValue;
}

void
Renderer::endFrame() {
	if (_context.textureUnit != -1) {
		glActiveTexture(GL_TEXTURE0);
		_context.textureUnit = -1;
	}
	if (_context.shaderId != -1) {
		glUseProgram(0);
		_context.shaderId = -1;
	}
	if (_context.vao != -1) {
		glBindVertexArray(0);
		_context.vao = -1;
	}
	_context.renderSettings.first = true;
}

void
Renderer::clearBuffers(int buffers) {
	clearBuffers(buffers, false);
}

void
Renderer::clearBuffers(int buffers, bool strict) {
	//RendererUtil::deallocateData();

	//RendererRecord* record = _context.getRendererRecord();
	RenderSettings& currentSettings = _context.renderSettings;
	int clear = 0;

	if (buffers & BUFFER_COLOR) {
		clear |= GL_COLOR_BUFFER_BIT;
	}

	if (buffers & BUFFER_DEPTH) {
		clear |= GL_DEPTH_BUFFER_BIT;
		if (!currentSettings.depthWrite) {
			glDepthMask(GL_TRUE);
			currentSettings.depthWrite = true;
		}
	}

	/*if (buffers & BUFFER_STENCIL) {
		clear |= GL_STENCIL_BUFFER_BIT;

		const GLuint STENCIL_MASK = ~((GLuint)0);
		if (currentSettings->getStencilFrontWriteMask() != STENCIL_MASK ||
			currentSettings->getStencilBackWriteMask() != STENCIL_MASK) {
			glStencilMask(STENCIL_MASK);
			currentSettings->setStencilWriteMask(STENCIL_MASK);
		}
	}*/

#ifdef never
	if (strict) {
		// grab our camera to get width and height info.
		Camera* cam = _context.camera;

		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, cam->getWidth(), cam->getHeight());
		_context.clippingTestEnabled = true;
	}
#endif

	glClear(clear);

	if (strict) {
		// put us back.
		applyScissors(&_context);
	}
}

void
Renderer::processQueue() {
	processQueue(true, true);
}

void
Renderer::processQueue(bool doSort, bool doClear) {
	_processingQueue = true;
	if (doSort && doClear) {
		for (auto p = _rq.begin(); p != _rq.end(); ++p) {
			p->second->sort();
			p->second->render();
			p->second->clear();
		}
	}
	else {
		if (doSort) {
			for (auto p = _rq.begin(); p != _rq.end(); ++p) {
				p->second->sort();
			}
		}
		for (auto p = _rq.begin(); p != _rq.end(); ++p) {
			p->second->render();
		}

		if (doClear) {
			for (auto p = _rq.begin(); p != _rq.end(); ++p) {
				p->second->clear();
			}
		}
	}
	_processingQueue = false;
}

void
Renderer::clearQueue() {
	for (auto p = _rq.begin(); p != _rq.end(); ++p) {
		p->second->clear();
	}
}

// XXX: Doesn't work correctly with submeshes.
void
Renderer::checkAndAdd(Renderable* s) {
	for (size_t i = 0; i < s->getNumSubmeshes(); ++i) {
		int index = s->getMaterial(i).getRenderQueue();
		map<int, unique_ptr<RenderQueue> >::iterator p = _rq.find(index);
		if (p == _rq.end()) {
			p = _rq.insert(make_pair(index, unique_ptr<RenderQueue>(new OpaqueRenderQueue(index, this)))).first;
		}
		p->second->add(i, s);
	}
}

void
Renderer::checkCardError() {
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		std::string errorString;
		switch (error) {
		case GL_INVALID_ENUM:
			errorString = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errorString = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errorString = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			errorString = "GL_OUT_OF_MEMORY";
			break;
		}
		throw Exception(__FILE__, __LINE__, "GL Error: " + errorString);
	}
}

void
Renderer::setViewport(int x, int y, int width, int height) {
	//LOG_T("setViewport %d %d %d %d", x, y, width, height);
	RenderContext* context = &_context;
	Rectangle2 vp(x, y, width, height);
	if (context->viewport != vp) {
		glViewport(x, y, width, height);
		context->viewport = vp;
	}
}

void
Renderer::setDepthRange(float depthRangeNear, float depthRangeFar) {
	RenderContext* context = &_context;
	if (context->depthRangeNear != depthRangeNear || context->depthRangeFar != depthRangeFar) {
		// NOTE: OpenGL ES may use glDepthRangef instead
#ifdef OPENGL_ES2
		glDepthRangef(depthRangeNear, depthRangeFar);
#else
		glDepthRange(depthRangeNear, depthRangeFar);
#endif
		context->depthRangeNear = depthRangeNear;
		context->depthRangeFar = depthRangeFar;
	}
}

void
Renderer::pushClip(const Rectangle2& rectangle) {
	RenderContext* context = &_context;
	context->clips.push_back(rectangle);

	applyScissors(context);
}

void
Renderer::pushEmptyClip() {
	RenderContext* context = &_context;
	context->clips.push_back(Rectangle2(-1 - 1, -1, -1));

	
	applyScissors(context);
}

void
Renderer::popClip() {
	RenderContext* context = &_context;
	context->clips.pop_back();

	applyScissors(context);
}

void
Renderer::clearClips() {
	RenderContext* context = &_context;
	context->clips.clear();

	applyScissors(context);
}

void
Renderer::setClipTestEnabled(bool enabled) {
	RenderContext* context = &_context;

	setClippingEnabled(context, enabled);
}

bool
Renderer::isClipTestEnabled() {
	RenderContext* context = &_context;
	return context->clippingTestEnabled;
}
