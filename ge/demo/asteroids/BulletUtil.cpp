/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <asteroids/BulletUtil.h>

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

using namespace asteroids;
using namespace std;

btTriangleMesh*
asteroids::buildBtTriangleMesh(const shared_ptr<ge::Mesh>& mesh) {
	btTriangleMesh* triMesh = new btTriangleMesh();
	for (size_t i = 0; i < mesh->triangles.size(); i += 3) {
		triMesh->addTriangle(fromGLM(mesh->vertices[mesh->triangles[i]]),
			fromGLM(mesh->vertices[mesh->triangles[i + 1]]),
			fromGLM(mesh->vertices[mesh->triangles[i + 2]]));
	}
	return triMesh;
}

// btBvhTriangleMeshShape is for static non-moving meshes.
btBvhTriangleMeshShape* 
asteroids::builtBtBvhTriangleMeshShape(const shared_ptr<ge::Mesh>& mesh) {
	return new btBvhTriangleMeshShape(buildBtTriangleMesh(mesh), true);
}

btConvexHullShape*
asteroids::buildBtConvexHull(const shared_ptr<ge::Mesh>& mesh, const glm::vec3& scale) {
	unique_ptr<btConvexHullShape> shape(new btConvexHullShape());
	for (size_t i = 0; i < mesh->triangles.size(); i++) {
		shape->addPoint(fromGLM(mesh->vertices[i] * scale));
	}

	// Optimize the convex hull.
	unique_ptr<btShapeHull> hull(new btShapeHull(shape.get()));
	hull->buildHull(shape->getMargin());

	return new btConvexHullShape(reinterpret_cast<const btScalar*>(hull->getVertexPointer()), hull->numVertices());
}

