/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <Config.h>

#include <ge/Mesh.h>

#include <btBulletDynamicsCommon.h>

#include <memory>

class btTriangleMesh;
class btBvhTriangleMeshShape;
class btConvexHullShape;
namespace wc {

inline btVector3 fromGLM(const glm::vec3& v) {
	return btVector3(v.x, v.y, v.z);
}

inline glm::vec3 toGLM(const btVector3& v) {
	return glm::vec3(v.getX(), v.getY(), v.getZ());
}

inline glm::vec4 toGLM4(const btVector3& v) {
	return glm::vec4(v.getX(), v.getY(), v.getZ(), 1.0f);
}

btTriangleMesh* buildBtTriangleMesh(const std::shared_ptr<ge::Mesh>& mesh);

// btBvhTriangleMeshShape is for static non-moving meshes.
std::unique_ptr<btBvhTriangleMeshShape> builtBtBvhTriangleMeshShape(const std::shared_ptr<ge::Mesh>& mesh);

std::unique_ptr<btConvexHullShape> buildBtConvexHull(const std::shared_ptr<ge::Mesh>& mesh, const glm::vec3& scale);
}