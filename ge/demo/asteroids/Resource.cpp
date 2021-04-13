/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <asteroids/Resource.h>

#include <ge/Mesh.h>
#include <ge/ShaderProgram.h>

#include <ge/InputStream.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

using namespace asteroids;
using namespace std;

namespace {
	void
		process(const vector<shared_ptr<ge::Mesh> >& meshes,
		const vector<unsigned int>& materialsMap,
		const vector<ge::Material>& materials,
		std::vector<shared_ptr<ge::MeshR >>& meshR,
		aiNode* node,
		aiMatrix4x4 accTransform) {

		aiMatrix4x4 transform = node->mTransformation * accTransform;
		// if node has meshes, create a new scene object for it
		if (node->mNumMeshes > 0) {
			for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
				shared_ptr<ge::MeshR> mr(new ge::MeshR());
				mr->setMesh(meshes[node->mMeshes[i]]);
				mr->setMaterial(materials[materialsMap[node->mMeshes[i]]]);

				aiVector3D scale;
				aiQuaternion rotation;
				aiVector3D position;
				transform.Decompose(scale, rotation, position);

				ge::Transform t;
				t.setRotation(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
				t.setTranslation(glm::vec3(position.x, position.y, position.z));
				t.setScale(glm::vec3(scale.x, scale.y, scale.z));
				mr->setTransform(t);

				meshR.push_back(mr);
			}
		}

		// Continue for all child nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			process(meshes, materialsMap, materials, meshR, node->mChildren[i], transform);
		}
	}
}
std::vector<shared_ptr<ge::MeshR> > asteroids::resource::loadobj(const string& name, const std::string& shader) {
	vector<shared_ptr<ge::MeshR> > meshR;

	// This code should move into the ge core, or perhaps a util library.
	Assimp::Importer i;
	const aiScene* scene = i.ReadFile(name.c_str(), aiProcess_Triangulate | aiProcess_GenNormals);
	if (scene == 0) {
		return meshR;
	}

	vector<unsigned int> materialsMap;
	vector<shared_ptr<ge::Mesh> > meshes;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		shared_ptr<ge::Mesh> m(new ge::Mesh());
		m->vertices.resize(scene->mMeshes[i]->mNumVertices);
		for (unsigned int n = 0; n < m->vertices.size(); ++n) {
			m->vertices[n] = glm::vec3(scene->mMeshes[i]->mVertices[n].x, scene->mMeshes[i]->mVertices[n].y, scene->mMeshes[i]->mVertices[n].z);
		}
		if (scene->mMeshes[i]->HasNormals()) {
			m->normals.resize(scene->mMeshes[i]->mNumVertices);
			for (unsigned int n = 0; n < m->normals.size(); ++n) {
				m->normals[n] = glm::vec3(scene->mMeshes[i]->mNormals[n].x, scene->mMeshes[i]->mNormals[n].y, scene->mMeshes[i]->mNormals[n].z);
			}
		}
		if (scene->mMeshes[i]->GetNumUVChannels() >= 1) {
			m->uv.resize(scene->mMeshes[i]->mNumVertices);
			for (unsigned int n = 0; n < m->uv.size(); ++n) {
				m->uv[n] = glm::vec2(scene->mMeshes[i]->mTextureCoords[0][n].x, scene->mMeshes[i]->mTextureCoords[0][n].y);
			}
		}

		// TODO: Handle things other than triangles.

		// Triangles.
		m->triangles.reserve(scene->mMeshes[i]->mNumFaces * 3);
		for (unsigned int n = 0; n < scene->mMeshes[i]->mNumFaces; n++) {
			const aiFace& Face = scene->mMeshes[i]->mFaces[n];
			assert(Face.mNumIndices == 3);
			m->triangles.push_back(Face.mIndices[0]);
			m->triangles.push_back(Face.mIndices[1]);
			m->triangles.push_back(Face.mIndices[2]);
		}

		// TODO: The code doesn't add the meshes to nodes as described in the scene. That's wrong.
		meshes.push_back(m);
		materialsMap.push_back(scene->mMeshes[i]->mMaterialIndex);
	}

	printf("material with %d textures\n", scene->mNumTextures);
	for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
		printf("texture %d,%d\n", scene->mTextures[i]->mWidth, scene->mTextures[i]->mHeight);
	}

	std::map<string, shared_ptr<sf::Texture> > textures;

	vector<ge::Material> materials;
	materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* mat = scene->mMaterials[i];
		materials[i] = ge::Material(ge::ShaderProgram::find(shader));
		materials[i].setColor("matDiffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		materials[i].setColor("matAmbient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		//printf("\nmaterial with %d properties\n", mat->mNumProperties);
		for (unsigned int p = 0; p < mat->mNumProperties; ++p) {
			const aiMaterialProperty* property = mat->mProperties[p];
			if (strcmp(property->mKey.data, _AI_MATKEY_TEXTURE_BASE) == 0) {
				//printf("0x%x ", property->mSemantic);
			}
			//printf("   %s\n", property->mKey.C_Str());
		}
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

				ge::FileInputStream stream;
				string path = "gnomeland-1.0/gnomeland-1.0/" + string(Path.C_Str());
				//string path = string(Path.C_Str());
				//size_t slashIndex = path.find_last_of('/');
				//if (slashIndex > 0) {
				//	path = path.substr(slashIndex + 1);
				//}
				auto tex = textures.find(path);
				if (tex != textures.end()) {
					materials[i].getTextureSettings0().texture = tex->second;
					materials[i].getTextureSettings0().enabled = true;
				}
				else {
					if (!stream.open(path)) {
						printf("cannot open %s", path);
						continue;
					}
					shared_ptr<sf::Texture> texture(new sf::Texture());
					if (!texture->loadFromStream(stream)) {
						printf("cannot load %s", path);
						continue;
					}
					textures.insert(make_pair(path, texture));

					materials[i].getTextureSettings0().texture = texture;
					materials[i].getTextureSettings0().enabled = true;
				}
			}
		}
		//aiString name;
		//aiGetMaterialString(mat, AI_MATKEY_NAME, &name);

		aiColor3D color(0.f, 0.f, 0.f);
		if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
			materials[i].setColor("matDiffuse", glm::vec4(color.r, color.g, color.b, 1.0f));
		}
	}

	aiMatrix4x4 t;
	process(meshes, materialsMap, materials, meshR, scene->mRootNode, t);

	return meshR;
}

