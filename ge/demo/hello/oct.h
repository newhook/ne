/**
 *
 * Copyright (c) 2014 Matthew Newhook. All rights reserved.
 *
 */

#include <ge/MeshR.h>
#include <Octree.hpp>

class OctreeAgentMesh : public hxa7241_graphics::OctreeAgent<ge::MeshR> {
public:
protected:
	virtual bool isOverlappingCell(const ge::MeshR& item,
		const hxa7241_graphics::Vector3r& lowerCorner,
		const hxa7241_graphics::Vector3r& upperCorner) const {
		ge::Bounds b;
		ge::Bounds b2 = item.getBounds();
		b.compute(glm::vec3(lowerCorner.getX(), lowerCorner.getY(), lowerCorner.getZ()), glm::vec3(upperCorner.getX(), upperCorner.getY(), upperCorner.getZ()));
		printf("octtree: c=%.2f,%.2f,%2.f e=%.2f, %.2f, %.2f intersects c=%.2f,%.2f,%2.f e=%.2f, %.2f, %.2f = %d\n", b.getCenter().x, b.getCenter().y, b.getCenter().z,
			b.getExtents().x, b.getExtents().y, b.getExtents().z,
			b2.getCenter().x, b2.getCenter().y, b2.getCenter().z,
			b2.getExtents().x, b2.getExtents().y, b2.getExtents().z,
			b.intersects(b2));

		return b.intersects(item.getBounds());
	}
};

class OctreeVisitorCamera
	: public hxa7241_graphics::OctreeVisitor<ge::MeshR>
{
	ge::Renderer* _r;
public:
	OctreeVisitorCamera(ge::Renderer* r) : _r(r) {
	}

private:
	OctreeVisitorCamera(const OctreeVisitorCamera&) {}
	OctreeVisitorCamera& operator=(const OctreeVisitorCamera&) { return *this; }


	/// commands -------------------------------------------------------------------
	/// octree visitor overrides
protected:
	virtual void  visitRoot(const hxa7241_graphics::OctreeCell* pRootCell,
		const hxa7241_graphics::OctreeData& octreeData) {
		hxa7241_graphics::OctreeRoot::continueVisit(pRootCell, octreeData, *this);
	}

	virtual void  visitBranch(const hxa7241_graphics::OctreeCell* subCells[8],
		const hxa7241_graphics::OctreeData& octreeData) {
		ge::Bounds b;
		const hxa7241_graphics::Vector3r& lowerCorner = octreeData.getBound().getLowerCorner();
		const hxa7241_graphics::Vector3r& upperCorner = octreeData.getBound().getUpperCorner();
		b.compute(glm::vec3(lowerCorner.getX(), lowerCorner.getY(), lowerCorner.getZ()), glm::vec3(upperCorner.getX(), upperCorner.getY(), upperCorner.getZ()));
		//if (ge::TestPlanesAABB(_r->getContext().camera->getPlanes(), b)) {
		//	for (int i = 0; i < 8; ++i) {
		//		hxa7241_graphics::OctreeBranch::continueVisit(subCells, octreeData, i, *this);
		//	}
		//}
	}

	virtual void  visitLeaf(const hxa7241_graphics::Array<const ge::MeshR*>& items,
		const hxa7241_graphics::OctreeData& octreeData) {
		ge::Bounds b;
		const hxa7241_graphics::Vector3r& lowerCorner = octreeData.getBound().getLowerCorner();
		const hxa7241_graphics::Vector3r& upperCorner = octreeData.getBound().getUpperCorner();
		b.compute(glm::vec3(lowerCorner.getX(), lowerCorner.getY(), lowerCorner.getZ()), glm::vec3(upperCorner.getX(), upperCorner.getY(), upperCorner.getZ()));
		/*if (TestPlanesAABB(_r->getContext().camera->getPlanes(), b)) {
			for (int i = 0, end = items.getLength(); i < end; ++i)
			{
				ge::MeshR* m = ((ge::MeshR*)items[i]);
				m->draw(_r);
			}
		}*/
	}
};

	
class OctreeVisitorDraw
	: public hxa7241_graphics::OctreeVisitor<ge::MeshR>
{
	ge::Renderer* _r;
	ge::ShaderProgram* _debugProg;
public:

	std::vector<ge::MeshR> render;

	OctreeVisitorDraw(ge::Renderer* r, ge::ShaderProgram* debugProg) : _r(r), _debugProg(debugProg) {
	}


	/// commands -------------------------------------------------------------------
	/// octree visitor overrides
protected:

	void add(const hxa7241_graphics::OctreeData& octreeData) {
		ge::Bounds b;
		const hxa7241_graphics::Vector3r& lowerCorner = octreeData.getBound().getLowerCorner();
		const hxa7241_graphics::Vector3r& upperCorner = octreeData.getBound().getUpperCorner();
		b.compute(glm::vec3(lowerCorner.getX(), lowerCorner.getY(), lowerCorner.getZ()), glm::vec3(upperCorner.getX(), upperCorner.getY(), upperCorner.getZ()));

		std::shared_ptr<ge::Mesh> bb = ge::pgeom::box(b.getCenter(), b.getExtents());
		bb->normals.clear();
		bb->uv.clear();

		ge::MeshR meshR;
		meshR.setMesh(bb);
		meshR.setMaterial(ge::Material(_debugProg));
		meshR.getMaterial().getRenderSettings().wireframe = true;
		meshR.getMaterial().getRenderSettings().depthTest = true;
		meshR.getMaterial().getRenderSettings().depthWrite = true;
		meshR.getMaterial().setRenderQueue(ge::RQ_OPAQUE + 1);
		render.push_back(meshR);
	}

	virtual void  visitRoot(const hxa7241_graphics::OctreeCell* pRootCell,
		const hxa7241_graphics::OctreeData& octreeData) {
		add(octreeData);
		hxa7241_graphics::OctreeRoot::continueVisit(pRootCell, octreeData, *this);
	}

	virtual void  visitBranch(const hxa7241_graphics::OctreeCell* subCells[8],
		const hxa7241_graphics::OctreeData& octreeData) {
		add(octreeData);
		for (int i = 0; i < 8; ++i) {
			hxa7241_graphics::OctreeBranch::continueVisit(subCells, octreeData, i, *this);
		}
	}

	virtual void  visitLeaf(const hxa7241_graphics::Array<const ge::MeshR*>& items,
		const hxa7241_graphics::OctreeData& octreeData) {
		add(octreeData);
	}
};
	