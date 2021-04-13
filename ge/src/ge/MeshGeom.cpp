/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/MeshGeom.h>
#include <ge/Mesh.h>
#include <memory>

using namespace ge;
using namespace std;

MeshGeom::MeshGeom(const Mesh& mesh) : _ivbo(0), _mode(mesh.getPrimitiveType()) {
	vbos.resize(GeometryType::MAX + 1);
	offsets.resize(GeometryType::MAX + 1);
	strides.resize(GeometryType::MAX + 1);

	size_t sz = 0;
	size_t stride = 0;

	instances = mesh.instances;
	if (instances[GeometryType::VERTICES] != 0) {
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), &mesh.vertices[0], GL_STATIC_DRAW);
		vbos[GeometryType::VERTICES] = vbo;
		offsets[GeometryType::VERTICES] = 0;
		strides[GeometryType::VERTICES] = 0;
	}
	else {
		sz = mesh.vertices.size() * sizeof(glm::vec3);
		stride = sizeof(glm::vec3);
	}

	if (!mesh.normals.empty()) {
		if (instances[GeometryType::NORMALS] != 0) {
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), &mesh.normals[0], GL_STATIC_DRAW);
			vbos[GeometryType::NORMALS] = vbo;
			offsets[GeometryType::NORMALS] = 0;
			strides[GeometryType::NORMALS] = 0;
		}
		else {
			sz += mesh.normals.size() * sizeof(glm::vec3);
			offsets[GeometryType::NORMALS] = stride;
			stride += sizeof(glm::vec3);
		}
	}
	if (!mesh.colors.empty()) {
		if (instances[GeometryType::COLORS] != 0) {
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh.colors.size() * sizeof(glm::vec4), &mesh.colors[0], GL_STATIC_DRAW);
			vbos[GeometryType::COLORS] = vbo;
			offsets[GeometryType::COLORS] = 0;
			strides[GeometryType::COLORS] = 0;
		}
		else {
			sz += mesh.colors.size() * sizeof(glm::vec4);
			offsets[GeometryType::COLORS] = stride;
			stride += sizeof(glm::vec4);
		}
	}
	if (!mesh.uv.empty()) {
		if (instances[GeometryType::UV] != 0) {
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh.uv.size() * sizeof(glm::vec2), &mesh.uv[0], GL_STATIC_DRAW);
			vbos[GeometryType::UV] = vbo;
			offsets[GeometryType::UV] = 0;
			strides[GeometryType::UV] = 0;
		}
		else {
			sz += mesh.uv.size() * sizeof(glm::vec2);
			offsets[GeometryType::UV] = stride;
			stride += sizeof(glm::vec2);
		}
	}
	if (!mesh.boneWeights.empty()) {
		if (instances[GeometryType::JOINTS] != 0) {
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh.uv2.size() * sizeof(glm::vec2), &mesh.uv2[0], GL_STATIC_DRAW);

			vbos[GeometryType::JOINTS] = vbo;
			offsets[GeometryType::JOINTS] = 0;
			strides[GeometryType::JOINTS] = 0;

			// WRONG
			vbos[GeometryType::WEIGHTS] = vbo;
			offsets[GeometryType::WEIGHTS] = 0;
			strides[GeometryType::WEIGHTS] = 0;
		}
		else {
			sz += 4 * sizeof(float);
			offsets[GeometryType::JOINTS] = stride;
			stride += sizeof(float);

			sz += 4 * sizeof(float);
			offsets[GeometryType::WEIGHTS] = stride;
			stride += sizeof(float);
		}
	}

	if (!mesh.uv2.empty()) {
		if (instances[GeometryType::UV2] != 0) {
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh.uv2.size() * sizeof(glm::vec2), &mesh.uv2[0], GL_STATIC_DRAW);
			vbos[GeometryType::UV2] = vbo;
			offsets[GeometryType::UV2] = 0;
			strides[GeometryType::UV2] = 0;
		}
		else {
			sz += mesh.uv2.size() * sizeof(glm::vec2);
			offsets[GeometryType::UV2] = stride;
			stride += sizeof(glm::vec2);
		}
	}

	if (sz > 0) {
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		unique_ptr<unsigned char[]> buf(new unsigned char[sz]);
		unsigned char* curr = buf.get() + offsets[GeometryType::VERTICES];
		if (instances[GeometryType::VERTICES] == 0) {
			for (size_t i = 0; i < mesh.vertices.size(); ++i) {
				*((glm::vec3*)curr) = mesh.vertices[i];
				curr += stride;
			}
			vbos[GeometryType::VERTICES] = vbo;
			strides[GeometryType::VERTICES] = stride;
		}
		if (!mesh.normals.empty() && instances[GeometryType::NORMALS] == 0) {
			curr = buf.get() + offsets[GeometryType::NORMALS];
			for (size_t i = 0; i < mesh.normals.size(); ++i) {
				*((glm::vec3*)curr) = mesh.normals[i];
				curr += stride;
			}
			vbos[GeometryType::NORMALS] = vbo;
			strides[GeometryType::NORMALS] = stride;
		}
		if (!mesh.colors.empty() && instances[GeometryType::COLORS] == 0) {
			curr = buf.get() + offsets[GeometryType::COLORS];
			for (size_t i = 0; i < mesh.colors.size(); ++i) {
				*((glm::vec4*)curr) = mesh.colors[i];
				curr += stride;
			}
			vbos[GeometryType::COLORS] = vbo;
			strides[GeometryType::COLORS] = stride;
		}
		if (!mesh.uv.empty() && instances[GeometryType::UV] == 0) {
			curr = buf.get() + offsets[GeometryType::UV];
			for (size_t i = 0; i < mesh.uv.size(); ++i) {
				*((glm::vec2*)curr) = mesh.uv[i];
				curr += stride;
			}
			vbos[GeometryType::UV] = vbo;
			strides[GeometryType::UV] = stride;
		}
		if (!mesh.uv2.empty() && instances[GeometryType::UV2] == 0) {
			curr = buf.get() + offsets[GeometryType::UV2];
			for (size_t i = 0; i < mesh.uv2.size(); ++i) {
				*((glm::vec2*)curr) = mesh.uv2[i];
				curr += stride;
			}
			vbos[GeometryType::UV2] = vbo;
			strides[GeometryType::UV2] = stride;
		}

		glBufferData(GL_ARRAY_BUFFER, sz, buf.get(), GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_submeshes = mesh.submeshes;
	if (_submeshes.empty()) {
		_submeshes.resize(1);
		_submeshes[0].indexOffset = 0;
		if (!mesh.triangles.empty()) {
			_submeshes[0].triangleCount = mesh.triangles.size() / 3;
		}
		else {
			_submeshes[0].triangleCount = mesh.vertices.size() / 3;
		}
	}
	if (!mesh.triangles.empty()) {
		glGenBuffers(1, &_ivbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ivbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(int), &mesh.triangles[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

MeshGeom::~MeshGeom() {
	vector<GLuint> v;
	for (size_t i = 0; i < vbos.size(); ++i) {
		if (vbos[i] != 0) {
			auto p = find(v.begin(), v.end(), vbos[i]);
			if (p == v.end()) {
				v.push_back(vbos[i]);
			}
		}
	}
	if (_ivbo != 0) {
		v.push_back(_ivbo);
	}
	if (!v.empty()) {
		glDeleteBuffers(v.size(), &v[0]);
	}

	// Clear all VBOs.
	for (size_t i = 0; i < vbos.size(); ++i) {
		vbos[i] = 0;
	}
	_ivbo = 0;
}

void
MeshGeom::render(size_t submesh) {
	GLsizei offset = _submeshes[submesh].indexOffset * sizeof(unsigned int);
	const GLsizei size = _submeshes[submesh].triangleCount * 3;
	if (_ivbo == 0) {
		glDrawArrays(_mode.getGLType(), offset, size);
	}
	else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ivbo);
		glDrawElements(_mode.getGLType(), size, GL_UNSIGNED_INT, (void*)offset);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

