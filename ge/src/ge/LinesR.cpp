/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Config.h>
#include <ge/LinesR.h>
#include <ge/Lines.h>
#include <ge/LineGeom.h>
#include <ge/Material.h>
#include <ge/Renderer.h>
#include <ge/ShaderAttribute.h>
#include <ge/Camera.h>
#include <ge/ShaderProgram.h>
#include <cassert>

using namespace ge;
using namespace std;

LinesR::LinesR() : _vao(0), _matrixDirty(true) {

}

LinesR::LinesR(const LinesR& other) : _vao(0) {
	_name = other._name;
	_geometry = other._geometry;
	_linesBounds = other._linesBounds;
	_bounds = other._bounds;
	_camera = other._camera;
	setMaterials(other._material);
	setTransform(other._transform);
}

LinesR&
LinesR::operator=(const LinesR& other) {
	if (this != &other) {
		if (_vao != 0) {
			glDeleteVertexArrays(1, &_vao);
			_vao = 0;
		}

		_name = other._name;
		_geometry = other._geometry;
		_linesBounds = other._linesBounds;
		_bounds = other._bounds;
		_camera = other._camera;

		setMaterials(other._material);
		setTransform(other._transform);
	}
	return *this;
}
LinesR::~LinesR() {
	if (_vao != 0) {
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}
}

void
LinesR::setLines(const shared_ptr<Lines>& lines) {
	if (!lines) {
		_geometry = 0;
		return;
	}

	_geometry = lines->getGeometry();
	lines->refreshBounds();
	_linesBounds = lines->getBounds();

	_bounds = _linesBounds;
	_bounds.transform(_transform);
}

void
LinesR::setMaterials(const vector<Material>& materials) {
	setMaterial(materials[0]);
}

void
LinesR::setMaterial(const Material& material) {
	_material.resize(1);
	_material[0] = material;

	if (_vao != 0) {
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}

	if (!_geometry) {
		return;
	}

	// TODO: The VAO can be created per material per mesh and then shared between all LinesR
	// since it will be the same.
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	const ShaderProgram* bundle = _material[0].getShader();
	assert(bundle->vertex);

	glBindBuffer(GL_ARRAY_BUFFER, _geometry->vbo);

	glEnableVertexAttribArray(bundle->vertex->getIndex());
	glVertexAttribPointer(bundle->vertex->getIndex(), 3, GL_FLOAT, GL_FALSE, _geometry->stride, reinterpret_cast<void*>(_geometry->offsets[GeometryType::VERTICES]));

	if (bundle->color) {
		
		glEnableVertexAttribArray(bundle->color->getIndex());
		glVertexAttribPointer(bundle->color->getIndex(), 4, GL_FLOAT, GL_FALSE, _geometry->stride, reinterpret_cast<void*>(_geometry->offsets[GeometryType::COLORS]));
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
LinesR::setTransform(const Transform& transform) {
	_matrixDirty = true;
	_transform = transform;
	
	_bounds = _linesBounds;
	_bounds.transform(_transform);
}

void
LinesR::draw(Renderer* r) {
	if (!_camera || !_geometry) {
		return;
	}

	if (_camera->getMode() != Camera::ORTHO && !TestPlanesAABB(_camera->getPlanes(), _bounds)) {
		return;
	}

	if (!r->isProcessingQueue()) {
		r->checkAndAdd(this);
	} else {
		render(0, r);
	}
}

void
LinesR::render(size_t submesh, Renderer* renderer) {
	if (_matrixDirty) {
		_worldMatrix = _transform.getWorldMatrix();
		_matrixDirty = false;
	}

	_material[0].bind(_worldMatrix, renderer, _camera, this);

	RenderContext& context = renderer->getContext();
	if (context.vao != _vao) {
		glBindVertexArray(_vao);
		context.vao = _vao;
	}

	_geometry->render();
}

glm::vec3
LinesR::getPosition() const {
	return _bounds.getCenter();
	//if (_worldBound) {
	//	return _worldBound->getCenter();
	//}
	//else {
	//	return _worldTransform.getTranslation();
	//}
}
