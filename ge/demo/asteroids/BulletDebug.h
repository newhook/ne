/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <LinearMath/btIDebugDraw.h>

#include <vector>
#include <memory>

namespace ge {
	class Renderer;
}
namespace asteroids {
	class RenderSystem;
	class BulletDebugDrawer : public btIDebugDraw
	{
		RenderSystem& _renderSystem;
		int _debugMode;

	public:

		BulletDebugDrawer(RenderSystem& renderSystem);
		virtual ~BulletDebugDrawer();

		virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

		virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

#ifdef never
		virtual void   drawSphere(const btVector3& p, btScalar radius, const btVector3& color);

		virtual void   drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);
#endif
		virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

		virtual void   reportErrorWarning(const char* warningString);

		virtual void   draw3dText(const btVector3& location, const char* textString);

		virtual void   setDebugMode(int debugMode);

		virtual int getDebugMode() const { return _debugMode; }

	};
}