/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Config.h>
#include <ge/PGeom.h>
#include <ge/Mesh.h>

using namespace ge;
using namespace std;

std::shared_ptr < ge::Mesh >
ge::pgeom::box(const glm::vec3& center, const glm::vec3& extents) {
	return box(center, extents.x, extents.y, extents.z);
}

shared_ptr<Mesh>
ge::pgeom::box(const glm::vec3 & center, float xExtent, float yExtent, float zExtent) {

	shared_ptr<Mesh> mesh(new Mesh());
	
	vector<glm::vec3> verts;
	verts.resize(8);
	verts[0] = center + glm::vec3(-xExtent, -yExtent, -zExtent);
	verts[1] = center + glm::vec3(xExtent, -yExtent, -zExtent);
	verts[2] = center + glm::vec3(xExtent, yExtent, -zExtent);
	verts[3] = center + glm::vec3(-xExtent, yExtent, -zExtent);
	verts[4] = center + glm::vec3(xExtent, -yExtent, zExtent);
	verts[5] = center + glm::vec3(-xExtent, -yExtent, zExtent);
	verts[6] = center + glm::vec3(xExtent, yExtent, zExtent);
	verts[7] = center + glm::vec3(-xExtent, yExtent, zExtent);

	vector<glm::vec3>& vertices = mesh->vertices;
	vertices.resize(24);
	
	// Back
	vertices[0] = verts[0];
	vertices[1] = verts[1];
	vertices[2] = verts[2];
	vertices[3] = verts[3];

	// Right
	vertices[4] = verts[1];
	vertices[5] = verts[4];
	vertices[6] = verts[6];
	vertices[7] = verts[2];

	// Front
	vertices[8] = verts[4];
	vertices[9] = verts[5];
	vertices[10] = verts[7];
	vertices[11] = verts[6];

	// Left
	vertices[12] = verts[5];
	vertices[13] = verts[0];
	vertices[14] = verts[3];
	vertices[15] = verts[7];

	// Top
	vertices[16] = verts[2];
	vertices[17] = verts[6];
	vertices[18] = verts[7];
	vertices[19] = verts[3];

	// Bottom
	vertices[20] = verts[0];
	vertices[21] = verts[5];
	vertices[22] = verts[4];
	vertices[23] = verts[1];

	vector<glm::vec3>& norms = mesh->normals;
	norms.resize(24);
	size_t i = 0;
	// back
	for (int c = 0; c < 4; ++c) {
		norms[i++] = glm::vec3(0, 0, -1);
	}
	// right
	for (int c = 0; c < 4; ++c) {
		norms[i++] = glm::vec3(1, 0, 0);
	}
	// front
	for (int c = 0; c < 4; ++c) {
		norms[i++] = glm::vec3(0, 0, 1);
	}
	// left
	for (int c = 0; c < 4; ++c) {
		norms[i++] = glm::vec3(-1, 0, 0);
	}
	// top
	for (int c = 0; c < 4; ++c) {
		norms[i++] = glm::vec3(0, 1, 0);
	}
	// bottom
	for (int c = 0; c < 4; ++c) {
		norms[i++] = glm::vec3(0, -1, 0);
	}

	vector<glm::vec2>& uv = mesh->uv;
	uv.resize(24);
	
	i = 0;
	for (int c = 0; c < 6; c++) {
		uv[i++] = glm::vec2(1, 0);
		uv[i++] = glm::vec2(0, 0);
		uv[i++] = glm::vec2(0, 1);
		uv[i++] = glm::vec2(1, 1);
	}

	const unsigned int indices[] =
	{ 2, 1, 0, 3, 2, 0, 6, 5, 4, 7, 6, 4, 10, 9, 8, 11, 10, 8, 14, 13, 12, 15, 14, 12,
	18, 17, 16, 19, 18, 16, 22, 21, 20, 23, 22, 20 };
	const size_t nindices = sizeof(indices) / sizeof(indices[0]);
	vector<unsigned int>& triangles = mesh->triangles;
	triangles.assign(indices, indices + nindices);

	return mesh;
}

std::shared_ptr<ge::Mesh>
pgeom::gridPlane(float horizontalExtend, float verticalExtend, int rows, int columns) {
	shared_ptr<ge::Mesh> mesh(new ge::Mesh());

	int numberVertices = (rows + 1) * (columns + 1);
	vector<glm::vec3>& vertices = mesh->vertices;
	vertices.resize(numberVertices);

	vector<glm::vec3>& normals = mesh->normals;
	normals.resize(numberVertices);

	vector<glm::vec2>& texCoords = mesh->uv;
	texCoords.resize(numberVertices);

	vector<unsigned int>& triangles = mesh->triangles;

	int numberIndices = rows * 6 * columns;
	triangles.resize(numberIndices);

	float  x, y, s, t;
	for (int i = 0; i < numberVertices; i++)
	{
		x = (float)(i % (columns + 1)) / (float)columns;
		y = 1.0f - (float)(i / (columns + 1)) / (float)rows;

		s = x;
		t = y;

		vertices[i].x = horizontalExtend * (x - 0.5f);
		vertices[i].y = verticalExtend * (y - 0.5f);
		vertices[i].z = 0.0f;

		normals[i].x = 0.0f;
		normals[i].y = 0.0f;
		normals[i].z = 1.0f;

		//tangents[i].x = 1.0f;
		//tangents[i].y = 0.0f;
		//tangents[i].z = 0.0f;

		texCoords[i].s = s;
		texCoords[i].t = t;
	}

	for (int i = 0; i < rows * columns; i++)
	{
		int currentColumn = i % columns;
		int currentRow = i / columns;

		triangles[i * 6 + 0] = currentColumn + currentRow * (columns + 1);
		triangles[i * 6 + 1] = currentColumn + (currentRow + 1) * (columns + 1);
		triangles[i * 6 + 2] = (currentColumn + 1) + (currentRow + 1) * (columns + 1);

		triangles[i * 6 + 3] = (currentColumn + 1) + (currentRow + 1) * (columns + 1);
		triangles[i * 6 + 4] = (currentColumn + 1) + currentRow * (columns + 1);
		triangles[i * 6 + 5] = currentColumn + currentRow * (columns + 1);
	}

	return mesh;
}

shared_ptr<Mesh>
pgeom::quad(float w, float h) {
	shared_ptr<Mesh> mesh(new Mesh());

	float w2 = w / 2;
	float h2 = h / 2;

	vector<glm::vec3>& vertices = mesh->vertices;
	vertices.resize(4);
	vertices[0] = glm::vec3(-w2, h2, 0);
	vertices[1] = glm::vec3(-w2, -h2, 0);
	vertices[2] = glm::vec3(w2, -h2, 0);
	vertices[3] = glm::vec3(w2, h2, 0);

	vector<glm::vec3>& normals = mesh->normals;
	normals.resize(4);
	normals[0] = glm::vec3(0, 0, 1);
	normals[1] = glm::vec3(0, 0, 1);
	normals[2] = glm::vec3(0, 0, 1);
	normals[3] = glm::vec3(0, 0, 1);

	vector<glm::vec2>& uv = mesh->uv;
	uv.resize(4);
	uv[0] = glm::vec2(0, 1);
	uv[1] = glm::vec2(0, 0);
	uv[2] = glm::vec2(1, 0);
	uv[3] = glm::vec2(1, 1);

	const unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	const size_t nindices = sizeof(indices) / sizeof(indices[0]);
	vector<unsigned int>& triangles = mesh->triangles;
	triangles.assign(indices, indices + nindices);

	return mesh;
}

shared_ptr<Mesh>
pgeom::quadXZ(float w, float h) {
	shared_ptr<Mesh> mesh(new Mesh());

	float w2 = w / 2;
	float h2 = h / 2;

	vector<glm::vec3>& vertices = mesh->vertices;
	vertices.resize(4);
	vertices[0] = glm::vec3(-w2, 0, h2);
	vertices[1] = glm::vec3(-w2, 0, -h2);
	vertices[2] = glm::vec3(w2, 0, -h2);
	vertices[3] = glm::vec3(w2, 0, h2);

	vector<glm::vec3>& normals = mesh->normals;
	normals.resize(4);
	normals[0] = glm::vec3(0, 0, 1);
	normals[1] = glm::vec3(0, 0, 1);
	normals[2] = glm::vec3(0, 0, 1);
	normals[3] = glm::vec3(0, 0, 1);

	vector<glm::vec2>& uv = mesh->uv;
	uv.resize(4);
	uv[0] = glm::vec2(0, 1);
	uv[1] = glm::vec2(0, 0);
	uv[2] = glm::vec2(1, 0);
	uv[3] = glm::vec2(1, 1);

	const unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	const size_t nindices = sizeof(indices) / sizeof(indices[0]);
	vector<unsigned int>& triangles = mesh->triangles;
	triangles.assign(indices, indices + nindices);

	return mesh;
}


shared_ptr<Mesh>
pgeom::sphere(int zSamples, int radialSamples, float radius, bool viewInside) {
	glm::vec3 _center = glm::vec3(0, 0, 0);
	const float M_PI = 3.14159265358979323846f;

	shared_ptr<Mesh> mesh(new Mesh());

	// allocate vertices
	int verts = (zSamples - 2) * (radialSamples + 1) + 2;

	vector<glm::vec3>& vertices = mesh->vertices;
	vertices.resize(verts);

	vector<glm::vec3>& normals = mesh->normals;
	normals.resize(verts);

	vector<glm::vec2>& uv = mesh->uv;
	uv.resize(verts);

	// generate geometry
	const float radialSamplesInv = 1.0f / radialSamples;
	const float zFactor = 2.0f / (zSamples - 1);

	// Generate points on the unit circle to be used in computing the mesh
	// points on a sphere slice.
	vector<float> unitSin(radialSamples + 1);
	vector<float> unitCos(radialSamples + 1);
	for (int i = 0; i < radialSamples; i++) {
		const float a = M_PI * 2.0f * radialSamplesInv * i;
		unitCos[i] = cos(a);
		unitSin[i] = sin(a);
	}
	unitSin[radialSamples] = unitSin[0];
	unitCos[radialSamples] = unitCos[0];

	// generate the sphere itself
	int i = 0;

	for (int zIndex = 1; zIndex < (zSamples - 1); ++zIndex) {
		float fAFraction = M_PI / 2 * (-1.0f + zFactor * zIndex);         // in (-pi/2, pi/2)
		float fZFraction = sin(fAFraction);         // in (-1,1)
		float fZ = radius * fZFraction;

		// compute center of slice
		glm::vec3 kSliceCenter = _center;
		kSliceCenter.z = (kSliceCenter.z + fZ);

		// compute radius of slice
		const float fSliceRadius = sqrt(abs(radius * radius - fZ * fZ));

		// compute slice vertices with duplication at end point
		
		int iSave = i;

		for (int radialIndex = 0; radialIndex < radialSamples; ++radialIndex) {

			float radialFraction = radialIndex * radialSamplesInv;             // in [0,1)
			glm::vec3 kRadial(unitCos[radialIndex], unitSin[radialIndex], 0);
			// TODO: XXX
			glm::vec3 tempVa = kRadial * fSliceRadius;
			tempVa = glm::vec3(kSliceCenter.x + tempVa.x, kSliceCenter.y + tempVa.y, kSliceCenter.z + tempVa.z);
			vertices[i] = tempVa;

			normals[i] = glm::normalize(tempVa - _center);
			if (viewInside) {
				normals[i] *= -1;
			}

			//if (_textureMode == TextureMode::Linear) {
			//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = fRadialFraction;
			//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f * (fZFraction + 1.0f);
			//}
			//else if (_textureMode == TextureMode::Projected) {
			//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = fRadialFraction;
			//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = MathUtils::INV_PI *
			//		(MathUtils::HALF_PI + asin(fZFraction));
			//}
			//else if (_textureMode == TextureMode::Polar) {
			//	const float r = (MathUtils::HALF_PI - abs(fAFraction)) / MathUtils::PI;
			//	const float u = r * afCos[iR] + 0.5f;
			//	const float v = r * afSin[iR] + 0.5f;
			//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = u;
			//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = v;
			//}
			uv[i] = glm::vec2(radialFraction, 0.5f * (fZFraction + 1.0f));

			i++;
		}

		vertices[i] = vertices[iSave];
		normals[i] = normals[iSave];

		//if (_textureMode == TextureMode::Linear) {
		//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 1.0f;
		//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f * (fZFraction + 1.0f);
		//}
		//else if (_textureMode == TextureMode::Projected) {
		//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 1.0f;
		//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = MathUtils::INV_PI *
		//		(MathUtils::HALF_PI + asin(fZFraction));
		//}
		//else if (_textureMode == TextureMode::Polar) {
		//	const float r = static_cast<float>((MathUtils::HALF_PI - abs(fAFraction)) / MathUtils::PI);
		//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = r + 0.5f;
		//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
		//}
		uv[i] = glm::vec2(1.0f, 0.5f * (fZFraction + 1.0f));

		i++;
	}

	// south pole
	vertices[i] = glm::vec3(_center.x, _center.y, _center.z - radius);

	if (!viewInside) {
		normals[i] = glm::vec3(0, 0, -1);
	}
	else {
		normals[i] = glm::vec3(0, 0, 1);
	}

	//if (_textureMode == TextureMode::Polar) {
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
	//}
	//else {
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.0f;
	//}
	uv[i] = glm::vec2(0.5f, 0.0f);
	i++;

	// north pole
	vertices[i] = glm::vec3(_center.x, _center.y, _center.z + radius);

	if (!viewInside) {
		normals[i] = glm::vec3(0, 0, 1);
	}
	else {
		normals[i] = glm::vec3(0, 0, -1);
	}

	//if (_textureMode == TextureMode::Polar) {
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
	//}
	//else {
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 0.5f;
	//	_meshData->getTextureBuffer(0)->data()[texBufPos++] = 1.0f;
	//}
	uv[i] = glm::vec2(0.5f, 1.0f);

	// Indices.

	// allocate connectivity
	const int tris = 2 * (zSamples - 2) * radialSamples;
	vector<unsigned int>& triangles = mesh->triangles;
	triangles.resize(tris * 3);

	i = 0;

	// generate connectivity
	int index = 0;
	for (int zIndex = 0, iZStart = 0; zIndex < (zSamples - 3); zIndex++) {
		int i0 = iZStart;
		int i1 = i0 + 1;
		iZStart += (radialSamples + 1);
		int i2 = iZStart;
		int i3 = i2 + 1;
		for (int radialIndex = 0; radialIndex < radialSamples; radialIndex++, index += 6) {
			if (!viewInside) {
				triangles[i++] = i0++;
				triangles[i++] = i1;
				triangles[i++] = i2;
				triangles[i++] = i1++;
				triangles[i++] = i3++;
				triangles[i++] = i2++;
			}             // inside view
			else{
				triangles[i++] = i0++;
				triangles[i++] = i2;
				triangles[i++] = i1;
				triangles[i++] = i1++;
				triangles[i++] = i2++;
				triangles[i++] = i3++;
			}
		}
	}

	// south pole triangles
	for (int radialIndex = 0; radialIndex < radialSamples; radialIndex++, index += 3) {
		if (!viewInside) {
			triangles[i++] = radialIndex;
			triangles[i++] = verts - 2;
			triangles[i++] = radialIndex + 1;
		}         // inside view
		else{
			triangles[i++] = radialIndex;
			triangles[i++] = radialIndex + 1;
			triangles[i++] = verts - 2;
		}
	}

	// north pole triangles
	const int iOffset = (zSamples - 3) * (radialSamples + 1);
	for (int radialIndex = 0; radialIndex < radialSamples; radialIndex++, index += 3) {
		if (!viewInside) {
			triangles[i++] = radialIndex + iOffset;
			triangles[i++] = radialIndex + 1 + iOffset;
			triangles[i++] = verts - 1;
		}         // inside view
		else{
			triangles[i++] = radialIndex + iOffset;
			triangles[i++] = verts - 1;
			triangles[i++] = radialIndex + 1 + iOffset;
		}
	}

	return mesh;
}