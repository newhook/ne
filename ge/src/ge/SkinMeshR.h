/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Renderable.h>
#include <ge/Transform.h>
#include <ge/Material.h>
#include <ge/Bounds.h>
#include <ge/MeshGeom.h>

#include <memory>

namespace ge {
class Mesh;
class SkinMeshR : public Renderable {
	std::string _name;
	std::vector<Material> _material;
	std::shared_ptr<MeshGeom> _geometry;
	bool _matrixDirty;
	glm::mat4 _worldMatrix;

	Transform _transform;

	Bounds _meshBounds;
	Bounds _bounds;

	std::vector<GLuint> _vao;
	std::vector<glm::mat4> _bones;

	void setMaterial(size_t submesh, const Material& material);

public:

	SkinMeshR();
	SkinMeshR(const SkinMeshR &);
	SkinMeshR & operator=(const SkinMeshR &);
	virtual ~SkinMeshR();

	void setBones(const std::vector<glm::mat4>& bones);

	virtual void setName(const std::string& name) {
		_name = name;
	}

	virtual const std::string& getName() const {
		return _name;
	}
	virtual size_t getNumSubmeshes() const {
		return _material.size();
	}

	virtual const Bounds& getBounds() const {
		return _bounds;
	}
	virtual void draw(Renderer* renderer);
	virtual void render(size_t submesh, Renderer* renderer);
	virtual glm::vec3 getPosition() const;

	void setMesh(const std::shared_ptr<Mesh>& mesh);
	const std::shared_ptr<MeshGeom>& getGeometry() const {
		return _geometry;
	}

	void updateMaterials();
	virtual void setMaterials(const std::vector<Material>& material);
	virtual void setMaterial(const Material& material);
	virtual Material& getMaterial(size_t submesh = 0) {
		return _material[submesh];
	}

	virtual Transform getTransform() const {
		return _transform;
	}
	virtual void setTransform(const Transform& transform);
};

}