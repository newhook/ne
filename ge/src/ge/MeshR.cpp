/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Config.h>
#include <ge/MeshR.h>
#include <ge/Mesh.h>
#include <ge/Material.h>
#include <ge/Renderer.h>
#include <ge/ShaderAttribute.h>
#include <ge/Camera.h>
#include <ge/ShaderProgram.h>
#include <cassert>

using namespace ge;
using namespace std;

void
MeshR::updateMaterials() {

}

void
MeshR::setMaterial(size_t submesh, const Material& material) {
	_material[submesh] = material;

	if (_vao[submesh] != 0) {
		glDeleteVertexArrays(1, &_vao[submesh]);
		_vao[submesh] = 0;
	}
	if (!_geometry) {
		return;
	}

	const ShaderProgram* bundle = _material[submesh].getShader();
	if (bundle == 0) {
		return;
	}

	// TODO: The VAO can be created per material per mesh and then shared between all MeshR
	// since it will be the same.
	glGenVertexArrays(1, &_vao[submesh]);
	glBindVertexArray(_vao[submesh]);

	assert(bundle->vertex);

	GLuint vbo = _geometry->vbos[GeometryType::VERTICES];
	GLuint boundVBO = vbo;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(bundle->vertex->getIndex());
	glVertexAttribPointer(bundle->vertex->getIndex(), 3, GL_FLOAT, GL_FALSE, _geometry->strides[GeometryType::VERTICES], reinterpret_cast<void*>(_geometry->offsets[GeometryType::VERTICES]));
	glVertexAttribDivisor(bundle->vertex->getIndex(), _geometry->instances[GeometryType::VERTICES]);

	if (bundle->normal && _geometry->vbos[GeometryType::NORMALS] != 0) {
		GLuint vbo = _geometry->vbos[GeometryType::NORMALS];
		if (vbo != boundVBO) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			boundVBO = vbo;
		}

		glEnableVertexAttribArray(bundle->normal->getIndex());
		glVertexAttribPointer(bundle->normal->getIndex(), 3, GL_FLOAT, GL_FALSE, _geometry->strides[GeometryType::NORMALS], reinterpret_cast<void*>(_geometry->offsets[GeometryType::NORMALS]));
		glVertexAttribDivisor(bundle->vertex->getIndex(), _geometry->instances[GeometryType::NORMALS]);
	}

	if (bundle->color && _geometry->vbos[GeometryType::COLORS] != 0) {
		GLuint vbo = _geometry->vbos[GeometryType::COLORS];
		if (vbo != boundVBO) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			boundVBO = vbo;
		}

		glEnableVertexAttribArray(bundle->color->getIndex());
		glVertexAttribPointer(bundle->color->getIndex(), 4, GL_FLOAT, GL_FALSE, _geometry->strides[GeometryType::COLORS], reinterpret_cast<void*>(_geometry->offsets[GeometryType::COLORS]));
		glVertexAttribDivisor(bundle->vertex->getIndex(), _geometry->instances[GeometryType::COLORS]);
	}

	if (bundle->uv&& _geometry->vbos[GeometryType::UV] != 0) {
		GLuint vbo = _geometry->vbos[GeometryType::UV];
		if (vbo != boundVBO) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			boundVBO = vbo;
		}

		glEnableVertexAttribArray(bundle->uv->getIndex());
		glVertexAttribPointer(bundle->uv->getIndex(), 2, GL_FLOAT, GL_FALSE, _geometry->strides[GeometryType::UV], reinterpret_cast<void*>(_geometry->offsets[GeometryType::UV]));
		glVertexAttribDivisor(bundle->vertex->getIndex(), _geometry->instances[GeometryType::UV]);
	}

	if (bundle->uv2 && _geometry->vbos[GeometryType::UV2] != 0) {
		GLuint vbo = _geometry->vbos[GeometryType::UV2];
		if (vbo != boundVBO) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			boundVBO = vbo;
		}

		glEnableVertexAttribArray(bundle->uv2->getIndex());
		glVertexAttribPointer(bundle->uv2->getIndex(), 2, GL_FLOAT, GL_FALSE, _geometry->strides[GeometryType::UV2], reinterpret_cast<void*>(_geometry->offsets[GeometryType::UV2]));
		glVertexAttribDivisor(bundle->vertex->getIndex(), _geometry->instances[GeometryType::UV2]);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

MeshR::MeshR() : _matrixDirty(true) {

}

MeshR::MeshR(const MeshR& other) {
	_name = other._name;
	_geometry = other._geometry;
	_meshBounds = other._meshBounds;
	_bounds = other._bounds;
	_camera = other._camera;
	setMaterials(other._material);
	setTransform(other._transform);
}

MeshR&
MeshR::operator=(const MeshR& other) {
	if (this != &other) {
		if (!_vao.empty()) {
			glDeleteVertexArrays(_vao.size(), &_vao[0]);
			_vao.clear();
		}		

		_name = other._name;
		_geometry = other._geometry;
		_meshBounds = other._meshBounds;
		_bounds = other._bounds;
		_camera = other._camera;
		setMaterials(other._material);
		setTransform(other._transform);
	}
	return *this;
}
MeshR::~MeshR() {
	if (!_vao.empty()) {
		glDeleteVertexArrays(_vao.size(), &_vao[0]);
		_vao.clear();
	}
}

void
MeshR::setMesh(const shared_ptr<Mesh>& mesh) {
	if (!mesh) {
		_geometry = 0;
		return;
	}

	_geometry = mesh->getGeometry();
	mesh->refreshBounds();
	_meshBounds = mesh->getBounds();

	_bounds = _meshBounds;
	_bounds.transform(_transform);
}

void
MeshR::setMaterials(const vector<Material>& materials) {
	// XXX: Free?
	_material.resize(materials.size());
	_vao.resize(materials.size());

		size_t submesh = 0;
	for (auto m : materials) {
		setMaterial(submesh++, m);
	}
}

void
MeshR::setMaterial(const Material& material) {
	_material.resize(1);
	_vao.resize(1);
	setMaterial(0, material);
}

void
MeshR::setTransform(const Transform& transform) {
	_matrixDirty = true;
	_transform = transform;
	_bounds = _meshBounds;
	_bounds.transform(_transform);
}

void
MeshR::draw(Renderer* r) {
	if (!_camera || !_geometry) {
		return;
	}

	if (_camera->getMode() != Camera::ORTHO && !TestPlanesAABB(_camera->getPlanes(), _bounds)) {
		return;
	}

	if (!r->isProcessingQueue()) {
		r->checkAndAdd(this);
	} else {
		for (size_t i = 0; i < _material.size(); ++i) {
			render(i, r);
		}
	}
}

void
MeshR::render(size_t submesh, Renderer* renderer) {
	// TODO: This method is the same as LinesR. Refactor.
	if (_matrixDirty) {
		_worldMatrix = _transform.getWorldMatrix();
		_matrixDirty = false;
	}
	_material[submesh].bind(_worldMatrix, renderer, _camera, this);

	RenderContext& context = renderer->getContext();
	if (context.vao != _vao[submesh]) {
		glBindVertexArray(_vao[submesh]);
		context.vao = _vao[submesh];
	}
	_geometry->render(submesh);
}

glm::vec3
MeshR::getPosition() const {
	return _bounds.getCenter();
}
