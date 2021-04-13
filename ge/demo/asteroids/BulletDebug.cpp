/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/BulletDebug.h>
#include <asteroids/BulletUtil.h>
#include <asteroids/RenderSystem.h>

#include <ge/LinesR.h>
#include <ge/ShaderProgram.h>

using namespace asteroids;

BulletDebugDrawer::BulletDebugDrawer(RenderSystem& renderSystem) : _renderSystem(renderSystem) {

}

BulletDebugDrawer::~BulletDebugDrawer() {

}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
	_renderSystem.drawLine(toGLM(from), toGLM(to), toGLM4(fromColor), toGLM4(toColor));
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	_renderSystem.drawLine(toGLM(from),  toGLM(to), toGLM4(color), toGLM4(color));
}

#ifdef never
void BulletDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color) {
}

void BulletDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) {
}

#endif
void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) { 
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString) { 
}
void BulletDebugDrawer::setDebugMode(int debugMode){
	_debugMode = debugMode;
}
