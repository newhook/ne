/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once

#include <Config.h>
#include <Resource.h>

#include <ge/Mesh.h>
#include <ge/ShaderProgram.h>

#include <ge/InputStream.h>
#include <ge/Animation.h>
#include <ge/Logger.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include <sstream>
#include <unordered_map>
#include <memory>

#include <fbxsdk.h>

using namespace wc;
using namespace std;

namespace {
ge::Logger logger(__FILE__);

struct ResourcePair {
	string name;
	string shader;
	ResourcePair(const string& name, const string& shader) {
		this->name = name;
		this->shader = shader;
	}
};

struct KeyHash {
	std::size_t operator()(const ResourcePair& k) const
	{
		return std::hash<std::string>()(k.name) ^
			(std::hash<std::string>()(k.shader) << 1);
	}
};
struct KeyEqual {
	bool operator()(const ResourcePair& lhs, const ResourcePair& rhs) const
	{
		return lhs.name == rhs.name && lhs.shader == rhs.shader;
	}
};
std::unordered_map<ResourcePair, shared_ptr<resource::Resource>, KeyHash, KeyEqual > resources;

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
				shared_ptr<ge::MeshR> mr = make_shared<ge::MeshR>();
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

shared_ptr<resource::Resource> loadobjImpl(const string& name, const std::string& shader) {
	shared_ptr<resource::Resource> rc = make_shared<resource::Resource>();

	ostringstream os;
	os << "models/" << name;

	// This code should move into the ge core, or perhaps a util library.
	Assimp::Importer i;
	const aiScene* scene = i.ReadFile(os.str().c_str(), aiProcess_Triangulate | aiProcess_GenNormals);
	if (scene == 0) {
		return rc;
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
		materials[i].setColor("m_diffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		materials[i].setColor("m_ambient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
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
			materials[i].setColor("m_diffuse", glm::vec4(color.r, color.g, color.b, 1.0f));
		}
	}

	aiMatrix4x4 t;
	process(meshes, materialsMap, materials, rc->meshR, scene->mRootNode, t);

	return rc;
}

// FBX importer.

// Get specific property value and connected texture if any.
// Value = Property value * Factor property value (if no factor property, multiply by 1).
FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
	const char * pPropertyName,
	const char * pFactorPropertyName,
	FbxFileTexture*& pTextureName)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}

	pTextureName = 0;
	if (lProperty.IsValid())
	{
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		if (lTextureCount)
		{
			pTextureName = lProperty.GetSrcObject<FbxFileTexture>();
		}
	}

	return lResult;
}

class GeFbxMaterial {
public:
	GeFbxMaterial(const FbxSurfaceMaterial* pMaterial) {
	}

	virtual ge::Material create(bool skinned) = 0;
};

class GeFbxLabertMaterial : public GeFbxMaterial  {

	glm::vec4 _emissive;
	FbxFileTexture* _emissiveTextureName;
	glm::vec4 _ambient;
	FbxFileTexture* _ambientTextureName;
	glm::vec4 _diffuse;
	FbxFileTexture* _diffuseTextureName;

protected:

	void setLambert(ge::Material& mat) {
		mat.setColor("m_diffuse", _diffuse);
		mat.setColor("m_ambient", _ambient);
		mat.setColor("m_emissive", _emissive);

		// Diffuse texture is bound to unit 0.
		mat.getTextureSettings0().enabled = true;
		shared_ptr<sf::Texture> diffuseTexture;
		if (_diffuseTextureName) {
			diffuseTexture = resource::loadTexture(_diffuseTextureName->GetFileName());
		}

		if (!diffuseTexture) {
			diffuseTexture = resource::whiteTexture();
		}

		mat.getTextureSettings0().texture = diffuseTexture;
	}

public:

	GeFbxLabertMaterial(const FbxSurfaceLambert* paterial) : GeFbxMaterial(paterial) {
		const FbxDouble3 emissive = GetMaterialProperty(paterial,
			FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, _emissiveTextureName);
		_emissive[0] = static_cast<GLfloat>(emissive[0]);
		_emissive[1] = static_cast<GLfloat>(emissive[1]);
		_emissive[2] = static_cast<GLfloat>(emissive[2]);

		const FbxDouble3 ambient = GetMaterialProperty(paterial,
			FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, _ambientTextureName);
		_ambient[0] = static_cast<GLfloat>(ambient[0]);
		_ambient[1] = static_cast<GLfloat>(ambient[1]);
		_ambient[2] = static_cast<GLfloat>(ambient[2]);

		const FbxDouble3 diffuse = GetMaterialProperty(paterial,
			FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, _diffuseTextureName);
		_diffuse[0] = static_cast<GLfloat>(diffuse[0]);
		_diffuse[1] = static_cast<GLfloat>(diffuse[1]);
		_diffuse[2] = static_cast<GLfloat>(diffuse[2]);
	}

	virtual ge::Material create(bool skinned) {
		string name = skinned ? "lambert_sk" : "lambert";
		ge::Material mat(ge::Material(ge::ShaderProgram::find(name)));
		setLambert(mat);
		return mat;
	}
};

class GeFbxPhongMaterial : public GeFbxLabertMaterial {
	glm::vec4 _specular;
	FbxFileTexture* _specularTextureName;
	float _shininess;

public:

	GeFbxPhongMaterial(const FbxSurfacePhong* material) : GeFbxLabertMaterial(material)  {
		const FbxDouble3 specular = GetMaterialProperty(material,
			FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, _specularTextureName);
		_specular[0] = static_cast<GLfloat>(specular[0]);
		_specular[1] = static_cast<GLfloat>(specular[1]);
		_specular[2] = static_cast<GLfloat>(specular[2]);

		FbxProperty shininessProperty = material->FindProperty(FbxSurfaceMaterial::sShininess);
		if (shininessProperty.IsValid())
		{
			double lShininess = shininessProperty.Get<FbxDouble>();
			_shininess = static_cast<GLfloat>(lShininess);
		}
	}

	virtual ge::Material create(bool skinned) {
		string name = skinned ? "phong_sk" : "phong";
		ge::Material mat(ge::Material(ge::ShaderProgram::find(name)));
		setLambert(mat);

		mat.setColor("m_specular", _specular);
		mat.setFloat("m_shininess", _shininess);

		return mat;
	}
};

class GeFbxImporter {
	vector<shared_ptr<ge::MeshR> > _meshR;
	vector<shared_ptr<ge::SkinMeshR> > _skinMeshR;
	map<FbxSurfaceMaterial*, shared_ptr<GeFbxMaterial> > _materials;
	std::vector<ge::Joint> _joints;
	std::vector<FbxNode*> _jointNode;

	short findJointIndex(const std::string& name) {
		short index = 0;
		for (auto p : _joints) {
			if (p.name == name) {
				return index;
			}
			++index;
		}
		return -1;
	}

	shared_ptr<ge::Mesh> process(const FbxMesh *pMesh) {
		shared_ptr<ge::Mesh> mesh = make_shared<ge::Mesh>();

		const int polygonCount = pMesh->GetPolygonCount();
		FbxLayerElementArrayTemplate<int>* materialIndices = NULL;
		FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;
		if (pMesh->GetElementMaterial()) {
			materialIndices = &pMesh->GetElementMaterial()->GetIndexArray();
			materialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
			if (materialIndices && materialMappingMode == FbxGeometryElement::eByPolygon) {
				FBX_ASSERT(materialIndices->GetCount() == polygonCount);
				if (materialIndices->GetCount() == polygonCount) {
					// Count the faces of each material
					for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
						const int materialIndex = materialIndices->GetAt(polygonIndex);
						if (mesh->submeshes.size() < materialIndex + 1) {
							mesh->submeshes.resize(materialIndex + 1);
						}
						mesh->submeshes[materialIndex].triangleCount += 1;
					}

					// Record the offset (how many vertex)
					const int materialCount = mesh->submeshes.size();
					int offset = 0;
					for (int i = 0; i < materialCount; ++i)
					{
						mesh->submeshes[i].indexOffset = offset;
						offset += mesh->submeshes[i].triangleCount * 3;
						// This will be used as counter in the following procedures, reset to zero
						mesh->submeshes[i].triangleCount = 0;
					}
					FBX_ASSERT(offset == polygonCount * 3);
				}
			}
		}

		// All faces will use the same material.
		if (mesh->submeshes.empty()) {
			mesh->submeshes.resize(1);
		}

		// Congregate all the data of a mesh to be cached in VBOs.
		// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
		bool hasNormals = pMesh->GetElementNormalCount() > 0;
		bool hasUVs = pMesh->GetElementUVCount() > 0;
		bool allByControlPoint;
		FbxGeometryElement::EMappingMode normalMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EMappingMode uvMappingMode = FbxGeometryElement::eNone;
		if (hasNormals) {
			normalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
			if (normalMappingMode == FbxGeometryElement::eNone) {
				hasNormals = false;
			}
			if (hasNormals && normalMappingMode != FbxGeometryElement::eByControlPoint) {
				allByControlPoint = false;
			}
		}
		if (hasUVs) {
			uvMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
			if (uvMappingMode == FbxGeometryElement::eNone) {
				hasUVs = false;
			}
			if (hasUVs && uvMappingMode != FbxGeometryElement::eByControlPoint) {
				allByControlPoint = false;
			}
		}

		// Allocate the array memory, by control point or by polygon vertex.
		int polygonVertexCount = pMesh->GetControlPointsCount();
		if (!allByControlPoint)
		{
			polygonVertexCount = polygonCount * 3;
		}

		mesh->vertices.resize(polygonVertexCount);
		mesh->triangles.resize(polygonCount * 3);
		if (hasNormals) {
			mesh->normals.resize(polygonVertexCount);
		}
		FbxStringList lUVNames;
		pMesh->GetUVSetNames(lUVNames);
		const char * lUVName = NULL;
		if (hasUVs && lUVNames.GetCount()) {
			mesh->uv.resize(polygonVertexCount);
			lUVName = lUVNames[0];
		}

		// Populate the array with vertex attribute, if by control point.
		const FbxVector4 * controlPoints = pMesh->GetControlPoints();
		FbxVector4 currentVertex;
		FbxVector4 currentNormal;
		FbxVector2 currentUV;
		if (allByControlPoint) {
			const FbxGeometryElementNormal * normalElement = NULL;
			const FbxGeometryElementUV * uvElement = NULL;
			if (hasNormals) {
				normalElement = pMesh->GetElementNormal(0);
			}
			if (hasUVs) {
				uvElement = pMesh->GetElementUV(0);
			}
			for (int i = 0; i < polygonVertexCount; ++i)
			{
				// Save the vertex position.
				currentVertex = controlPoints[i];
				mesh->vertices[i].x = static_cast<float>(currentVertex[0]);
				mesh->vertices[i].y = static_cast<float>(currentVertex[1]);
				mesh->vertices[i].z = static_cast<float>(currentVertex[2]);

				// Save the normal.
				if (hasNormals)
				{
					int lNormalIndex = i;
					if (normalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lNormalIndex = normalElement->GetIndexArray().GetAt(i);
					}
					currentNormal = normalElement->GetDirectArray().GetAt(lNormalIndex);

					mesh->normals[i].x = static_cast<float>(currentNormal[0]);
					mesh->normals[i].y = static_cast<float>(currentNormal[1]);
					mesh->normals[i].z = static_cast<float>(currentNormal[2]);
				}

				// Save the UV.
				if (hasUVs)
				{
					int lUVIndex = i;
					if (uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lUVIndex = uvElement->GetIndexArray().GetAt(i);
					}
					currentUV = uvElement->GetDirectArray().GetAt(lUVIndex);
					mesh->uv[i].x = static_cast<float>(currentUV[0]);
					mesh->uv[i].y = static_cast<float>(currentUV[1]);
				}
			}
		}

		multimap<int, size_t> pointVert;
		int vertexCount = 0;
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
			// The material for current face.
			int materialIndex = 0;
			if (materialIndices && materialMappingMode == FbxGeometryElement::eByPolygon) {
				materialIndex = materialIndices->GetAt(polygonIndex);
			}

			// Where should I save the vertex attribute index, according to the material
			const int indexOffset = mesh->submeshes[materialIndex].indexOffset +
				mesh->submeshes[materialIndex].triangleCount * 3;
			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
				const int lControlPointIndex = pMesh->GetPolygonVertex(polygonIndex, vertexIndex);

				pointVert.insert(make_pair(lControlPointIndex, indexOffset + vertexIndex));
				if (allByControlPoint) {
					mesh->triangles[indexOffset + vertexIndex] = static_cast<unsigned int>(lControlPointIndex);
				}
				else {
					// Populate the array with vertex attribute, if by polygon vertex.
					mesh->triangles[indexOffset + vertexIndex] = static_cast<unsigned int>(vertexCount);
					
					currentVertex = controlPoints[lControlPointIndex];
					mesh->vertices[vertexCount].x = static_cast<float>(currentVertex[0]);
					mesh->vertices[vertexCount].y = static_cast<float>(currentVertex[1]);
					mesh->vertices[vertexCount].z = static_cast<float>(currentVertex[2]);

					if (hasNormals) {
						pMesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, currentNormal);
						mesh->normals[vertexCount].x = static_cast<float>(currentNormal[0]);
						mesh->normals[vertexCount].y = static_cast<float>(currentNormal[1]);
						mesh->normals[vertexCount].z = static_cast<float>(currentNormal[2]);
					}

					if (hasUVs) {
						bool lUnmappedUV;
						pMesh->GetPolygonVertexUV(polygonIndex, vertexIndex, lUVName, currentUV, lUnmappedUV);
						mesh->uv[vertexCount].x = static_cast<float>(currentUV[0]);
						mesh->uv[vertexCount].y = static_cast<float>(currentUV[1]);
					}
				}
				++vertexCount;
			}
			mesh->submeshes[materialIndex].triangleCount += 1;
		}

		struct BoneWeight {
			short index;
			float weight;
			bool operator<(const BoneWeight& other) const {
				return weight > other.weight;
			}
		};

		vector<vector<BoneWeight> > boneWeights;

		// If the FbxMesh has skin deformers then it is a skinned mesh.
		const int numSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (numSkinDeformers > 0) {
			boneWeights.resize(mesh->vertices.size());
		}

		for (int t = 0; t < numSkinDeformers; ++t) {
			const FbxSkin* pSkin = (FbxSkin*)(pMesh->GetDeformer(0, FbxDeformer::eSkin));
			const int numClusters = pSkin->GetClusterCount();

			for (int clusterIdx = 0; clusterIdx < numClusters; ++clusterIdx) {
				const FbxCluster* pCluster = pSkin->GetCluster(clusterIdx);
				//obtain the cluster(bone) info
				const char* pClusterName = pCluster->GetLink()->GetName();
				short jointIndex = findJointIndex(pClusterName);
				if (jointIndex == -1) {
					LOG_O("unknown joint `%s'", pClusterName)
					continue;
				}

				// The weights.
				const int numAffectedVertices = pCluster->GetControlPointIndicesCount();
				const int* arrControlPointsIndices = pCluster->GetControlPointIndices();
				const double* arrWeights = pCluster->GetControlPointWeights();

				for (int i = 0; i < numAffectedVertices; ++i) {
					auto vertex = pointVert.equal_range(arrControlPointsIndices[i]);
					for (auto p = vertex.first; p != vertex.second; ++p) {
						BoneWeight bw;
						bw.index = jointIndex;
						bw.weight = arrWeights[i];
						boneWeights[p->second].push_back(bw);
					}
				}
			}
		}

		if (!boneWeights.empty()) {
			mesh->boneWeights.resize(boneWeights.size());
			for (size_t i = 0; i < boneWeights.size(); ++i) {
				if (boneWeights[i].empty()) {
					continue;
				}

				vector<BoneWeight> bw = boneWeights[i];
				sort(bw.begin(), bw.end());
				float w = 0.0f;
				while (bw.size() > 4) {
					w += bw[bw.size()-1].weight;
					bw.pop_back();
				}
				if (w > 0.0f) {
					float r = w / 4.0f;
					for (auto p : bw) {
						p.weight += r;
					}
				}

				ge::BoneWeight& b = mesh->boneWeights[i];
				b.index0 = bw[0].index;
				b.weight0 = bw[0].weight;
				if (bw.size() > 1) {
					b.index1 = bw[1].index;
					b.weight1 = bw[1].weight;
				}
				if (bw.size() > 2) {
					b.index2 = bw[2].index;
					b.weight2 = bw[2].weight;
				}
				if (bw.size() > 3) {
					b.index3 = bw[3].index;
					b.weight3 = bw[3].weight;
				}
			}
		}
		return mesh;
	}

	void processBindPoseMesh(const FbxMesh *pMesh) {
		// If the FbxMesh has skin deformers then it is a skinned mesh.
		const int numSkinDeformers = pMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int t = 0; t < numSkinDeformers; ++t) {
			const FbxSkin* pSkin = (FbxSkin*)(pMesh->GetDeformer(0, FbxDeformer::eSkin));
			const int numClusters = pSkin->GetClusterCount();

			for (int clusterIdx = 0; clusterIdx < numClusters; ++clusterIdx) {
				const FbxCluster* pCluster = pSkin->GetCluster(clusterIdx);
				//obtain the cluster(bone) info
				const char* pClusterName = pCluster->GetLink()->GetName();
				short jointIndex = findJointIndex(pClusterName);
				if (jointIndex == -1) {
					LOG_O("unknown joint `%s'", pClusterName)
						continue;
				}

				// Inverse bind pose for the joint.
				FbxAMatrix transformMatrix;
				FbxAMatrix transformLinkMatrix;
				FbxAMatrix globalBindposeInverseMatrix;

				pCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
				pCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
				globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix;

				FbxVector4 position = globalBindposeInverseMatrix.GetT();
				FbxVector4 scale = globalBindposeInverseMatrix.GetS();
				FbxQuaternion rotation = globalBindposeInverseMatrix.GetQ();

				ge::Transform& t = _joints[jointIndex].inverseBindPose;

				t.setRotation(glm::quat(rotation[3], rotation[0], rotation[1], rotation[2]));
				t.setTranslation(glm::vec3(position[0], position[1], position[2]));
				t.setScale(glm::vec3(scale[0], scale[1], scale[2]));
			}
		}
	}

public:

	const std::vector<shared_ptr<ge::MeshR> >& getMeshRenderers() const {
		return _meshR;
	}
	const std::vector<shared_ptr<ge::SkinMeshR> >& getSkinMeshRenderers() const {
		return _skinMeshR;
	}

	const std::vector<ge::Joint> getJoints() const { return _joints;}
	const std::vector<FbxNode*>& getJointNodes() const { return _jointNode; }

	void processSkeleton(FbxNode* fbxNode, short depth, short index, short parentIndex) {
		FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();
		if (nodeAttribute) {
			// Bake mesh as VBO(vertex buffer object) into GPU.
			FbxNodeAttribute::EType type = nodeAttribute->GetAttributeType();
			if (type == FbxNodeAttribute::eSkeleton) {
				FbxSkeleton* fbxSkeleton = fbxNode->GetSkeleton();
				if (fbxSkeleton) {
					ge::Joint joint;
					joint.name = fbxNode->GetName();
					joint.parentIndex = parentIndex;
					joint.index = static_cast<short>(index);
					_joints.push_back(joint);
					_jointNode.push_back(fbxNode);
				}
			}
		}

		const int childCount = fbxNode->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex) {
			processSkeleton(fbxNode->GetChild(childIndex), depth++, static_cast<short>(_joints.size()), index);
		}
	}

	bool processBindPose(FbxNode* fbxNode) {
		bool rc = false;
		FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();
		if (nodeAttribute) {
			// Bake mesh as VBO(vertex buffer object) into GPU.
			FbxNodeAttribute::EType type = nodeAttribute->GetAttributeType();
			if (type == FbxNodeAttribute::eMesh) {
				FbxMesh* fbxMesh = fbxNode->GetMesh();
				if (fbxMesh) {
					if (fbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0) {
						processBindPoseMesh(fbxMesh);
						rc = true;
					}
				}
			}
		}

		const int childCount = fbxNode->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex) {
			rc |= processBindPose(fbxNode->GetChild(childIndex));
		}
		return rc;
	}

	void process(FbxNode* fbxNode) {
		vector<shared_ptr<GeFbxMaterial> > materials;
		const int materialCount = fbxNode->GetMaterialCount();
		materials.resize(materialCount);
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
			FbxSurfaceMaterial * material = fbxNode->GetMaterial(materialIndex);

			auto p = _materials.find(material);
			if (p == _materials.end()) {
				string shadingModel = material->ShadingModel.Get();
				if (shadingModel == "Lambert") {
					FbxSurfaceLambert* lambert = FbxCast<FbxSurfaceLambert>(material);
					p = _materials.insert(make_pair(material, make_shared<GeFbxLabertMaterial>(lambert))).first;
				}
				else {
					FbxSurfacePhong* phong = FbxCast<FbxSurfacePhong>(material);
					p = _materials.insert(make_pair(material, make_shared<GeFbxPhongMaterial>(phong))).first;
				}
			}
			materials[materialIndex] = p->second;
		}

		FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();
		if (nodeAttribute) {
			// Bake mesh as VBO(vertex buffer object) into GPU.
			FbxNodeAttribute::EType type = nodeAttribute->GetAttributeType();
			if (type == FbxNodeAttribute::eMesh) {
				FbxMesh* fbxMesh = fbxNode->GetMesh();
				if (fbxMesh) {
					shared_ptr<ge::Mesh> mesh = process(fbxMesh);
					FbxAMatrix& transform = fbxNode->EvaluateGlobalTransform();

					FbxVector4 position = transform.GetT();
					FbxVector4 scale = transform.GetS();
					FbxQuaternion rotation = transform.GetQ();

					ge::Transform t;
					t.setRotation(glm::quat(rotation[3], rotation[0], rotation[1], rotation[2]));
					t.setTranslation(glm::vec3(position[0], position[1], position[2]));
					t.setScale(glm::vec3(scale[0], scale[1], scale[2]));

					if (!mesh->boneWeights.empty()) {
						shared_ptr<ge::SkinMeshR> renderable = make_shared<ge::SkinMeshR>();
						renderable->setName(fbxNode->GetName());
						vector<ge::Material> mat;
						for (auto fbxMat : materials) {
							mat.push_back(fbxMat->create(true));
						}
						renderable->setMesh(mesh);
						renderable->setMaterials(mat);

						renderable->setTransform(t);

						_skinMeshR.push_back(renderable);

					}
					else {
						shared_ptr<ge::MeshR> renderable = make_shared<ge::MeshR>();
						renderable->setName(fbxNode->GetName());
						vector<ge::Material> mat;
						for (auto fbxMat : materials) {
							mat.push_back(fbxMat->create(false));
						}
						renderable->setMesh(mesh);
						renderable->setMaterials(mat);

						renderable->setTransform(t);

						_meshR.push_back(renderable);
					}
				}
			}
		}

		const int childCount = fbxNode->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex) {
			process(fbxNode->GetChild(childIndex));
		}
	}
};
}

shared_ptr<resource::Resource> loadfbx(const string& name, const std::string& shader) {
	// TODO: The FbxManager stuff should be done once.

	// Initialize the SDK manager. This object handles memory management.
	FbxManager* sdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* fbxImporter = FbxImporter::Create(sdkManager, "");

	ostringstream os;
	os << "models/" << name;

	// Use the first argument as the filename for the importer.
	if (!fbxImporter->Initialize(os.str().c_str(), -1, sdkManager->GetIOSettings())) {
		// TODO: Correct logging.
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", fbxImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(sdkManager, "myScene");

	// Import the contents of the file into the scene.
	fbxImporter->Import(scene);

	// TODO: Exception safe.

	// The file is imported, so get rid of the importer.
	fbxImporter->Destroy();

	// Convert Axis System to what is used in this example, if needed
	FbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
	if (SceneAxisSystem != OurAxisSystem) {
		OurAxisSystem.ConvertScene(scene);
	}

	// Convert Unit System to what is used in this example, if needed
	FbxSystemUnit SceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
	if (SceneSystemUnit.GetScaleFactor() != 1.0) {
		//The unit in this example is centimeter.
		FbxSystemUnit::cm.ConvertScene(scene);
	}

	// Get the list of all the animation stack.
	//mScene->FillAnimStackNameArray(mAnimStackNameArray);

	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter geomConverter(sdkManager);
	geomConverter.Triangulate(scene, /*replace*/true);

	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	geomConverter.SplitMeshesPerMaterial(scene, true);

	GeFbxImporter geImp;

	shared_ptr<resource::Resource> rc = make_shared<resource::Resource>();

	// Fills in the joints.
	geImp.processSkeleton(scene->GetRootNode(), 0, 0, -1);
	if (geImp.processBindPose(scene->GetRootNode())) {
		rc->skeleton = make_shared<ge::Skeleton>(geImp.getJoints());
	}

	geImp.process(scene->GetRootNode());

	FbxArray<FbxString*> mAnimStackNameArray;
	scene->FillAnimStackNameArray(mAnimStackNameArray);
	const int lAnimStackCount = mAnimStackNameArray.GetCount();
	if (lAnimStackCount > 0) {
		rc->animation = make_shared<ge::AnimationClip>(geImp.getJointNodes().size());
		for (int i = 0; i < lAnimStackCount; ++i) {
			// select the base layer from the animation stack
			FbxAnimStack * lCurrentAnimationStack = scene->FindMember<FbxAnimStack>(mAnimStackNameArray[i]->Buffer());
			if (lCurrentAnimationStack != NULL) {
				// we assume that the first animation layer connected to the animation stack is the base layer
				// (this is the assumption made in the FBXSDK)
				FbxAnimLayer* mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
				scene->SetCurrentAnimationStack(lCurrentAnimationStack);

				FbxTime mStart;
				FbxTime mStop;
				FbxTakeInfo* lCurrentTakeInfo = scene->GetTakeInfo(*(mAnimStackNameArray[i]));
				if (lCurrentTakeInfo) {
					mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
					mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
				}
				else {
					// Take the time line value
					FbxTimeSpan lTimeLineTimeSpan;
					scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

					mStart = lTimeLineTimeSpan.GetStart();
					mStop = lTimeLineTimeSpan.GetStop();
				}
				float start = mStart.GetSecondDouble();

				// TODO: Probably really want 60, but 24 is fine for now.
				for (FbxLongLong frameIndex = mStart.GetFrameCount(FbxTime::eFrames24); frameIndex <= mStop.GetFrameCount(FbxTime::eFrames24); ++frameIndex)
				{
					FbxTime currTime;
					currTime.SetFrame(frameIndex, FbxTime::eFrames24);

					for (size_t jointIndex = 0; jointIndex < geImp.getJointNodes().size(); ++jointIndex) {
						FbxAMatrix transform = geImp.getJointNodes()[jointIndex]->EvaluateLocalTransform(currTime);

						FbxVector4 position = transform.GetT();
						FbxVector4 scale = transform.GetS();
						FbxQuaternion rotation = transform.GetQ();

						rc->animation->getChannel(jointIndex).getData().push_back(ge::AnimationData(glm::quat(rotation[3], rotation[0], rotation[1], rotation[2]),
							glm::vec3(position[0], position[1], position[2])));
						rc->animation->getChannel(jointIndex).getTimes().push_back(currTime.GetSecondDouble() - start);
					}
				}
			}
		}
	}

	rc->skinMeshR = geImp.getSkinMeshRenderers();
	rc->meshR = geImp.getMeshRenderers();

	return rc;
}

shared_ptr<resource::Resource>  wc::resource::loadobj(const string& name, const std::string& shader) {
	auto p = resources.find(ResourcePair(name, shader));
	if (p == resources.end()) {
		if (name.find(".fbx") != string::npos) {
			auto r = loadfbx(name, shader);
			if (r) {
				p = resources.insert(make_pair(ResourcePair(name, shader), r)).first;
			}
		}
		else {
			auto r = loadobjImpl(name, shader);
			if (r) {
				p = resources.insert(make_pair(ResourcePair(name, shader), r)).first;
			}
		}
	}

	if (p == resources.end()) {
		return nullptr;
	}
	return p->second;
}

namespace {
shared_ptr<sf::Texture> white;
}
shared_ptr<sf::Texture> wc::resource::whiteTexture() {
	if (!white) {
		sf::Image img;
		img.create(1, 1, sf::Color::White);
		white = make_unique<sf::Texture>();
		white->loadFromImage(img);
	}
	return white;
}
namespace {
string getLastComponent(const string& name) {
	size_t pos = name.find_last_of('/');
	if (pos != string::npos) {
		return name.substr(pos + 1);
	}
	pos = name.find_last_of('\\');
	if (pos != string::npos) {
		return name.substr(pos + 1);
	}
	return name;
}
set<string> texturePaths;
}

void wc::resource::addTextureLocation(const std::string& path) {
	texturePaths.insert(path);
}

// TODO: cache.
shared_ptr<sf::Texture> wc::resource::loadTexture(const string& fullname) {
	ge::FileInputStream stream;
	string name = getLastComponent(fullname);
	bool opened = false;
	for (auto path : texturePaths) {
		if (stream.open(path + "/" + name)) {
			opened = true;
			break;
		}
	}
	if (!opened) {
		printf("cannot open `%s'", fullname);
		return nullptr;
	}

	shared_ptr<sf::Texture> texture(new sf::Texture());
	if (!texture->loadFromStream(stream)) {
		printf("cannot load `%s'", name);
		return nullptr;
	}
	return texture;
}

shared_ptr<sf::Image> wc::resource::loadImage(const string& name) {
	ge::FileInputStream stream("models");
	if (!stream.open(name)) {
		printf("cannot open `%s'", name);
		return nullptr;
	}
	shared_ptr<sf::Image> image(new sf::Image());
	if (!image->loadFromStream(stream)) {
		printf("cannot load `%s'", name);
		return nullptr;
	}
	return image;
}

std::vector<std::shared_ptr<ge::MeshR> > wc::resource::Resource::getMeshR() const {
	std::vector<std::shared_ptr<ge::MeshR> > r;
	for (auto p : meshR) {
		r.push_back(make_shared<ge::MeshR>(*p));
	}
	return r;
}

std::vector<std::shared_ptr<ge::SkinMeshR> > wc::resource::Resource::getSkinMeshR() const {
	std::vector<std::shared_ptr<ge::SkinMeshR> > r;
	for (auto p : skinMeshR) {
		r.push_back(make_shared<ge::SkinMeshR>(*p));
	}
	return r;
}