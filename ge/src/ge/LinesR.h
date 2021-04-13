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

#include <memory>

namespace ge {
class Lines;
class LineGeom;
class LinesR : public Renderable {
	std::string _name;
	std::vector<Material> _material;
	std::shared_ptr<LineGeom> _geometry;
	bool _matrixDirty;
	glm::mat4 _worldMatrix;

	Transform _transform;

	Bounds _linesBounds;
	Bounds _bounds;

	GLuint _vao;

public:

	LinesR();
	LinesR(const LinesR&);
	LinesR& operator=(const LinesR&);
	virtual ~LinesR();

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

	void setLines(const std::shared_ptr<Lines>& lines);
	const std::shared_ptr<LineGeom>& getGeometry() const {
		return _geometry;
	}

	virtual void setMaterials(const std::vector<Material>& material);

	virtual void setMaterial(const Material& material);
	virtual Material& getMaterial(size_t submesh = 0) {
		return _material[submesh];
	}

	virtual Transform getTransform() const {
		return _transform;
	}
	virtual void setTransform(const Transform& transform);

	void bind();
	void unbind();
};

}