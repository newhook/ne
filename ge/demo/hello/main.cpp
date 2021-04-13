/**
 *
 * Copyright (c) 2014 Matthew Newhook. All rights reserved.
 *
 */

#include <SFML/Graphics.hpp>
#include <ge/Renderer.h>
#include <ge/MeshR.h>
#include <ge/PGeom.h>
#include <ge/PsShader.h>
#include <ge/VsShader.h>
#include <ge/ShaderProgram.h>
#include <ge/Material.h>
#include <ge/InputStream.h>
#include <ge/Camera.h>
#include <ge/Mesh.h>

#include <MouseOrbitZoom.h>
#include <CameraControl.h>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include <oct.h>

#include <artemis/World.h>
#include <artemis/SystemManager.h>
#include <artemis/EntityManager.h>
#include <artemis/EntityProcessingSystem.h>
#include <artemis/ComponentMapper.h>
#include <artemis/Entity.h>

#include <memory>
#include <random>

using namespace std;

inline btVector3
convert(const glm::vec3& v) {
	return btVector3(v.x, v.y, v.z);
}

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

std::vector<shared_ptr<ge::MeshR> >
loadobj(const string& name) {
	vector<shared_ptr<ge::MeshR> > meshR;
	// This code should move into the ge core, or perhaps a util library.
	Assimp::Importer i;
	vector<unsigned int> materialsMap;
	vector<shared_ptr<ge::Mesh> > meshes;
	const aiScene* scene = i.ReadFile(name.c_str(), aiProcess_Triangulate | aiProcess_GenNormals);
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
		materials[i] = ge::Material(ge::ShaderProgram::find("diffuse"));
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

shared_ptr<ge::Mesh>
createBoxMesh() {
	static shared_ptr<ge::Mesh> boxMesh;
	if (!boxMesh) {
		boxMesh = ge::pgeom::box(glm::vec3(0, 0, 0), 1, 1, 1);
	}
	return boxMesh;
}

class TransformComponent : public artemis::Component
{
public:
	TransformComponent(const ge::Transform& t) {
		transform = t;
	}
	ge::Transform transform;
};

class PhysicsComponent : public artemis::Component
{
public:
	btCollisionShape* shape;
	btRigidBody* rigid;

	// The group to which this component belongs.
	short collisionGroup;
	// The collision mask.
	short collisionMask;

	PhysicsComponent(btCollisionShape* shape, btRigidBody* rigid)
	{
		this->shape = shape;
		this->rigid = rigid;
		this->collisionGroup = 0;
		this->collisionMask = 0;
	}

	PhysicsComponent(btCollisionShape* shape, btRigidBody* rigid, short group, short mask)
	{
		this->shape = shape;
		this->rigid = rigid;
		this->collisionGroup = group;
		this->collisionMask = mask;
	}
};

class PlayerComponent : public artemis::Component
{
public:
	btKinematicCharacterController* controller;

	PlayerComponent(btKinematicCharacterController* controller)
	{
		this->controller = controller;
	}
};

class RenderComponent : public artemis::Component
{
public:
	std::vector<shared_ptr<ge::Renderable> > renderable;

	RenderComponent(const shared_ptr<ge::Renderable>& renderable)
	{
		this->renderable.push_back(renderable);
	}

	RenderComponent(const std::vector<shared_ptr<ge::Renderable> >& renderable)
	{
		this->renderable = renderable;
	}

	RenderComponent(const std::vector<shared_ptr<ge::MeshR> >& renderable)
	{
		for (auto p = renderable.begin(); p != renderable.end(); ++p) {
			this->renderable.push_back(*p);
		}
	}
};

// TODO: Bullet and such should live in the physics system.
class PlayerSystem : public artemis::EntityProcessingSystem
{
	artemis::ComponentMapper<PlayerComponent> playerMapper;
	artemis::ComponentMapper<TransformComponent> transformMapper;

	const sf::Window& _window;
	ge::Camera* _camera;

	float _sensitivityX;
	float _sensitivityY;

	float _minimumX;
	float _maximumX;

	float _minimumY;
	float _maximumY;

	float _rotationY;

	glm::vec3 _localEuler;

public:

	PlayerSystem(const sf::Window& window, ge::Camera* camera) : _window(window), _camera(camera) {
		setComponentTypes<PlayerComponent, TransformComponent>();

		_sensitivityX = 0.1f;
		_sensitivityY = 0.1f;

		_minimumX = -360.0f;
		_maximumX = 360.0f;

		_minimumY = -60.0f;
		_maximumY = 60.0f;

		_rotationY = 0.0f;
	}

	virtual void initialize() {
		playerMapper.init(*world);
		transformMapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		PlayerComponent* component = playerMapper.get(e);

		// Update the transform based on the physics system.
		btTransform trans = component->controller->getGhostObject()->getWorldTransform();
		TransformComponent* transform = transformMapper.get(e);
		transform->transform = trans;

		// Follow camera.
		sf::Vector2i center;
		center.x = _window.getSize().x / 2;
		center.y = _window.getSize().y / 2;

		sf::Vector2i mousePosition = sf::Mouse::getPosition(_window);
		mousePosition -= center;

		//_camera->setTransform(transform->transform);

		_rotationY += mousePosition.y * _sensitivityY;

		_localEuler = glm::vec3(-_rotationY, 0, 0);

		ge::Transform cameraTransform = _camera->getTransform();
		cameraTransform.setTranslation(cameraTransform.getTranslation() - -glm::vec3(1.0f, 1.75f, 5.0f));		
		cameraTransform.lookAt(transform->transform);
		//_camera->setTransform(cameraTransform);

		// Move the player.
		glm::vec3 dir;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))  {
			dir.z = -1.0;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))  {
			dir.z = 1.0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))  {
			dir.x = -1.0;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))  {
			dir.x = 1.0;
		}

		if (dir == glm::vec3()) {
			dir = glm::vec3(0.0001, 0, 0.0001);
		}
		else {
			dir *= 0.25;
		}
		
		// TODO: It would be better perhaps to use setVelocityForTimeInterval? 
		component->controller->setWalkDirection(btVector3(dir.x, _rotationY, dir.z));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))  {
			component->controller->jump();
		}
	}
};


class ExpireComponent : public artemis::Component {
public:

	ExpireComponent(float t) {
		time = t;
	}

	float time;
};

// TODO: Bullet and such should live in the physics system.
class PhysicsSystem : public artemis::EntityProcessingSystem
{
	btDiscreteDynamicsWorld* _world;
	artemis::ComponentMapper<PhysicsComponent> physicsMapper;
	artemis::ComponentMapper<TransformComponent> transformMapper;

public:

	PhysicsSystem(btDiscreteDynamicsWorld* world) : _world(world) {
		setComponentTypes<PhysicsComponent>();
	}

	virtual void initialize() {
		physicsMapper.init(*world);
		transformMapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		TransformComponent* transform = transformMapper.get(e);
		if (transform) {
			PhysicsComponent* physics = physicsMapper.get(e);
			btTransform trans;
			physics->rigid->getMotionState()->getWorldTransform(trans);
			transform->transform = trans;
		}
	}

	virtual void added(artemis::Entity &e) {
		PhysicsComponent* physics = physicsMapper.get(e);
		_world->addRigidBody(physics->rigid, physics->collisionGroup, physics->collisionMask);
	}

	virtual void removed(artemis::Entity &e) {
		PhysicsComponent* physics = physicsMapper.get(e);
		_world->removeRigidBody(physics->rigid);
	}
};

int seed = 100;
std::mt19937 random(seed);

void
createExplosion(artemis::EntityManager* em, const glm::vec3& position) {
	int pieces = 10;

	for (int i = 0; i < pieces; ++i) {
		glm::vec3 size(0.3f, 0.3f, 0.3f);

		shared_ptr<ge::MeshR> meshR(new ge::MeshR());
		meshR->setMesh(createBoxMesh());

		meshR->setMaterial(ge::Material(ge::ShaderProgram::find("diffuse")));

		// Set the lighting uniforms.
		meshR->getMaterial().setColor("matDiffuse", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
		meshR->getMaterial().setColor("matAmbient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
		glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		meshR->getMaterial().setColor("lightDiffuse", lightDiffuse);
		meshR->getMaterial().setColor("lightAmbient", lightAmbient);

		meshR->setTransform(meshR->getTransform().setTranslation(position).setScale(size));

		btCollisionShape* shape = new btBoxShape(convert(size));

		float y = 40 + ((float)random() / random.max()) * 10.0f;
		float x = 0 + ((float)random() / random.max()) * 360.0f;
		float z = 0 + ((float)random() / random.max()) * 360.0f;

		glm::quat rot(glm::vec3(x, 70.0f + y, z));
		btDefaultMotionState* fallMotionState = new btDefaultMotionState(
			btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), convert(position)));

		btScalar mass = 0.1;
		//btVector3 fallInertia(0, 0, 0);
		btVector3 fallInertia(0, 70.0f + y, 0);
		shape->calculateLocalInertia(mass, fallInertia);

		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, shape, fallInertia);
		btRigidBody* rigidBody = new btRigidBody(fallRigidBodyCI);
		rigidBody->applyForce(btVector3(x, 70.0f + y, z), btVector3(0,0,0));

		artemis::Entity & box = em->create();
		box.addComponent(new PhysicsComponent(shape, rigidBody));
		box.addComponent(new TransformComponent(meshR->getTransform()));
		box.addComponent(new RenderComponent(meshR));
		box.addComponent(new ExpireComponent(3.0f));
		box.refresh();

		printf("created new box %d\n", box.getId());

		shape->setUserPointer(&box);
	}
}


class CollisionSystem : public artemis::EntitySystem
{
	btCollisionDispatcher* _dispatcher;
	std::list<std::pair<artemis::Entity*, artemis::Entity*> > _pairs;

public:

	CollisionSystem(btCollisionDispatcher* dispatcher) : _dispatcher(dispatcher) {
	}

	virtual void initialize() {
	}

	virtual bool checkProcessing() {
		return true;
	}

	virtual void begin() {
		int numManifolds = _dispatcher->getNumManifolds();
		for (int i = 0; i < numManifolds; i++) {
			btPersistentManifold* contactManifold = _dispatcher->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++) {
				artemis::Entity* entity0 = (artemis::Entity*)obA->getCollisionShape()->getUserPointer();
				artemis::Entity* entity1 = (artemis::Entity*)obB->getCollisionShape()->getUserPointer();
				if (entity0 != 0 && entity1 != 0) {
					_pairs.push_back(make_pair(entity0, entity1));
				}
			}

			contactManifold->clearManifold();
		}
	}

	virtual void end() {
		_pairs.clear();
	}

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*> & bag) {
		for (auto p = _pairs.begin(); p != _pairs.end(); ++p) {
			if ((world->getGroupManager()->getGroupOf(*p->first) == "WORLD" &&
				world->getGroupManager()->getGroupOf(*p->second) == "BOX") ||
				(world->getGroupManager()->getGroupOf(*p->first) == "BOX" &&
				world->getGroupManager()->getGroupOf(*p->second) == "WORLD")) {

				artemis::Entity* box;
				if (world->getGroupManager()->getGroupOf(*p->first) == "BOX") {
					box = p->first;
				}
				else {
					box = p->second;
				}

				createExplosion(world->getEntityManager(), box->getComponent<TransformComponent>()->transform.getTranslation());
				world->deleteEntity(*box);
			}
		}
	}
};

class RotateComponent : public artemis::Component
{
public:

	glm::quat rotate;
	float angle;

	RotateComponent()
	{
		this->angle = 0.0f;
	}
};

class RotateSystem : public artemis::EntityProcessingSystem
{
	artemis::ComponentMapper<RotateComponent> rotateMapper;
	artemis::ComponentMapper<TransformComponent> transformMapper;
	const glm::vec3 ROTATING_AXIS = glm::normalize(glm::vec3(1, 1, 0.5));
	const glm::vec3 NORM_AXIS = glm::normalize(ROTATING_AXIS);

	glm::quat quatFromAngleNormalAxis(float angle, const glm::vec3& axis) {
		angle *= 0.5f;
		float s = sin(angle);
		float c = cos(angle);
		return glm::quat(c, axis.x * s, axis.y * s, axis.z * s);
	}

public:

	RotateSystem() {
		setComponentTypes<RotateComponent, TransformComponent>();
	}

	virtual void initialize() {
		rotateMapper.init(*world);
		transformMapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		RotateComponent* rotate = rotateMapper.get(e);

		rotate->angle = static_cast<float>(fmod(rotate->angle + world->getDelta() * 50, 360));
		rotate->rotate = quatFromAngleNormalAxis(rotate->angle * 3.14f / 180.0f, NORM_AXIS);

		TransformComponent* transform = transformMapper.get(e);
		transform->transform.setRotation(quatFromAngleNormalAxis(rotate->angle * 3.14f / 180.0f, NORM_AXIS));
	}
};


class RenderSystem : public artemis::EntityProcessingSystem
{
	ge::Renderer* _renderer;
	ge::Camera* _camera;

	artemis::ComponentMapper<RenderComponent> _renderMapper;
	artemis::ComponentMapper<TransformComponent> _transformMapper;

	bool _changeWireframe;
	bool _wireframe;
	bool _showBounds;
	std::vector<ge::MeshR> _boundsMeshR;

	glm::vec4 _lightDir;

public:

	RenderSystem(ge::Renderer* renderer, ge::Camera* camera) : _renderer(renderer), _camera(camera), _changeWireframe(false), _wireframe(false), _showBounds(false) {
		setComponentTypes<RenderComponent>();
	}

	virtual void initialize() {
		_renderMapper.init(*world);
		_transformMapper.init(*world);
	}

	bool getWireframe() const {
		return _wireframe;
	}
	void setWireframe(bool v) {
		_changeWireframe = true;
		_wireframe = v;
	}

	bool getShowBounds() const {
		return _showBounds;
	}

	void setShowBounds(bool v) {
		_showBounds = v;
	}

	virtual void processEntity(artemis::Entity &e) {

		// octree stuff.
		//octree.visit(visitor);

		//visitorDraw.render.clear();
		//octree.visit(visitorDraw);
		//for (auto p = visitorDraw.render.begin(); p != visitorDraw.render.end(); ++p) {
		//	p->draw(&renderer);
		//}


		RenderComponent* component = _renderMapper.get(e);
		TransformComponent* transform = _transformMapper.get(e);
		for (auto p = component->renderable.begin(); p != component->renderable.end(); ++p) {

			// Update the renderable position if there is a transform component.
			if (transform) {
				(*p)->setTransform(transform->transform);

				//octree.remove(meshR, agent);
				//octree.insert(meshR, agent);
			}

			if (_changeWireframe) {
				(*p)->getMaterial().getRenderSettings().wireframe = _wireframe;
			}

			if (_showBounds) {
				ge::Bounds b = (*p)->getBounds();
				shared_ptr<ge::Mesh> bb = ge::pgeom::box(b.getCenter(), b.getExtents());
				bb->normals.clear();
				bb->uv.clear();

				ge::MeshR bmr;
				bmr.setMesh(bb);
				bmr.setCamera(_camera);
				bmr.setMaterial(ge::Material(ge::ShaderProgram::find("debug")));
				bmr.getMaterial().getRenderSettings().wireframe = true;
				bmr.getMaterial().getRenderSettings().depthTest = true;
				bmr.getMaterial().getRenderSettings().depthWrite = true;
				bmr.getMaterial().setRenderQueue(ge::RQ_OPAQUE + 1);

				_boundsMeshR.push_back(bmr);
			}

			(*p)->getMaterial().setColor("lightDirection", _lightDir);

			(*p)->draw(_renderer);
		}
	}

	void RenderSystem::added(artemis::Entity &e) {
		RenderComponent* renderComponent = _renderMapper.get(e);
		if (renderComponent) {
			for (auto p = renderComponent->renderable.begin(); p != renderComponent->renderable.end(); ++p) {
				if (!(*p)->getCamera()) {
					(*p)->setCamera(_camera);
				}
			}
		}
	}

	virtual void begin() {
		_lightDir = _camera->getModelViewMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		_boundsMeshR.clear();
	}

	virtual void end() {
		_changeWireframe = false;
		for (auto p = _boundsMeshR.begin(); p != _boundsMeshR.end(); ++p) {
			p->draw(_renderer);
		}
		_renderer->processQueue();
	}
};

class ExpirationSystem : public artemis::EntityProcessingSystem
{
	artemis::ComponentMapper<ExpireComponent> mapper;

public:

	ExpirationSystem() {
		setComponentTypes<ExpireComponent>();
	}

	virtual void initialize() {
		mapper.init(*world);
	}

	virtual void processEntity(artemis::Entity &e) {
		ExpireComponent* component = mapper.get(e);
		component->time -= world->getDelta();
		if (component->time < 0) {
			printf("deleting %d\n", e.getId());
			world->deleteEntity(e);
		}
	}
};

short COLL_BOX = 0x01;
short COLL_WORLD = 0x02;

artemis::Entity&
createBox(ge::ShaderProgram* prog, artemis::EntityManager* em, const glm::vec3& position) {
	shared_ptr<ge::MeshR> meshR(new ge::MeshR());
	meshR->setMesh(createBoxMesh());

	meshR->setMaterial(ge::Material(prog));

	// Set the lighting uniforms.
	meshR->getMaterial().setColor("matDiffuse", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
	meshR->getMaterial().setColor("matAmbient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
	glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	meshR->getMaterial().setColor("lightDiffuse", lightDiffuse);
	meshR->getMaterial().setColor("lightAmbient", lightAmbient);

	meshR->setTransform(meshR->getTransform().setTranslation(position));

	btCollisionShape* shape = new btBoxShape(btVector3(1, 1, 1));
	
	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	shape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, shape, fallInertia);
	btRigidBody* rigidBody = new btRigidBody(fallRigidBodyCI);

	artemis::Entity & box = em->create();
	box.addComponent(new PhysicsComponent(shape, rigidBody, COLL_BOX, COLL_WORLD));
	box.addComponent(new TransformComponent(meshR->getTransform()));
	box.addComponent(new RenderComponent(meshR));
	box.setGroup("BOX");
	box.refresh();

	printf("created new box %d", box.getId());

	shape->setUserPointer(&box);

	return box;
}

// btBvhTriangleMeshShape is for static non-moving meshes.
btBvhTriangleMeshShape*
createBvhTriangleMeshShape(const shared_ptr<ge::Mesh>& mesh) {
#ifdef never
	unique_ptr<btVector3> verts(new btVector3[mesh->vertices.size()]);
	btVector3* vv = verts.get();
	for (size_t i = 0; i < mesh->vertices.size(); ++i) {
		vv[i].setX(mesh->vertices[i].x);
		vv[i].setY(mesh->vertices[i].y);
		vv[i].setZ(mesh->vertices[i].z);
	}

	btTriangleIndexVertexArray *va = new btTriangleIndexVertexArray(
		mesh->triangles.size() / 3, reinterpret_cast<int*>(&mesh->triangles[0]), 3 * sizeof(int),
		mesh->vertices.size(), (btScalar*)verts.get(), sizeof(btVector3));
	return new btBvhTriangleMeshShape(va, true);
#endif
	btTriangleMesh* triMesh = new btTriangleMesh();
	for (size_t i = 0; i < mesh->triangles.size(); i += 3) {
		triMesh->addTriangle(convert(mesh->vertices[mesh->triangles[i]]),
			convert(mesh->vertices[mesh->triangles[i + 1]]),
			convert(mesh->vertices[mesh->triangles[i + 2]]));
	}
	return new btBvhTriangleMeshShape(triMesh, true);
}

bool appInFocus(sf::RenderWindow* app) {
	if (app == NULL)
		return false;

	HWND handle = app->getSystemHandle();
	bool one = handle == GetFocus();
	bool two = handle == GetForegroundWindow();

	if (one != two) //strange 'half-focus': window is in front but you can't click anything - so we fix it
	{
		SetFocus(handle);
		SetForegroundWindow(handle);
	}

	return one && two;
}

void
printmat4(const glm::mat4& m) {
	printf("%.2f %.2f %.2f %.2f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
	printf("%.2f %.2f %.2f %.2f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
	printf("%.2f %.2f %.2f %.2f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
	printf("%.2f %.2f %.2f %.2f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

void
printplane(const ge::Plane& p) {
	printf("[%.2f %.2f %.2f] %.2f\n", p.getNormal().x, p.getNormal().y, p.getNormal().z, p.getConstant());
}

int main(int argc, char** argv)
{
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");

	if (glewInit() != GLEW_OK) {
		// XXX:
	}

	unique_ptr<btDbvtBroadphase> broadphase(new btDbvtBroadphase());
	unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration(new btDefaultCollisionConfiguration());	
	unique_ptr<btCollisionDispatcher> dispatcher(new btCollisionDispatcher(collisionConfiguration.get()));
	unique_ptr<btSequentialImpulseConstraintSolver> solver(new btSequentialImpulseConstraintSolver());
	unique_ptr<btDiscreteDynamicsWorld> world(new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get()));
	world->setGravity(btVector3(0, -9.8f, 0));

	ge::Renderer renderer;
	ge::Camera camera;

	artemis::World aWorld;
	artemis::SystemManager * sm = aWorld.getSystemManager();
	PhysicsSystem& physicsSystem = (PhysicsSystem&)sm->setSystem(new PhysicsSystem(world.get()));
	RenderSystem& renderSystem = (RenderSystem&)sm->setSystem(new RenderSystem(&renderer, &camera));
	RotateSystem& rotateSystem = (RotateSystem&)sm->setSystem(new RotateSystem());
	PlayerSystem& playerSystem = (PlayerSystem&)sm->setSystem(new PlayerSystem(window, &camera));
	ExpirationSystem& expirationSystem = (ExpirationSystem&)sm->setSystem(new ExpirationSystem());
	CollisionSystem& collisionSystem = (CollisionSystem&)sm->setSystem(new CollisionSystem(dispatcher.get()));
	artemis::EntityManager * em = aWorld.getEntityManager();

	sm->initializeAll();

	camera.setFOV(45.0f);
	camera.setViewport(0, 0, window.getSize().x, window.getSize().y);
	camera.setClipping(1, 1000);

	camera.setTransform(camera.getTransform().setTranslation(glm::vec3(0.0f, 50.0f, 20.0f)).lookAt(glm::vec3(0.0f, 0.0f, 0.0f)));

	glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
	glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	unique_ptr<CameraControl> cameraControl(new CameraControl(&camera));

	// Load an axis. XYZ axis where X is red, Y is green and Z is blue. The axis points
	// in the positive direction.
	{
		std::vector<shared_ptr<ge::MeshR> > shape = loadobj("axis.nff");
		artemis::Entity & entity = em->create();

		for (auto p = shape.begin(); p != shape.end(); ++p) {
			ge::Transform t = (*p)->getTransform();
			(*p)->setTransform(t.setTranslation(t.getTranslation() + glm::vec3(5, 5, 5)));
			(*p)->getMaterial().setColor("lightDiffuse", lightDiffuse);
			(*p)->getMaterial().setColor("lightAmbient", lightAmbient);
		}

		entity.addComponent(new RenderComponent(shape));
		entity.refresh();
	}

	// Load the teapot.
	{
		std::vector<shared_ptr<ge::MeshR> > shape = loadobj("teapot.obj");
		artemis::Entity & entity = em->create();
		for (auto p = shape.begin(); p != shape.end(); ++p) {
			(*p)->getMaterial().setColor("lightDiffuse", lightDiffuse);
			(*p)->getMaterial().setColor("lightAmbient", lightAmbient);
		}
		entity.addComponent(new RenderComponent(shape));
		entity.refresh();

		// Set the target for the camera.
		//std::static_pointer_cast<MouseOrbitZoom>(cameraControl->getBehaviorByType(CameraControl::Orbit))->setTarget(shape[0]);
	}
	
	// Create the world box.
	{
		// 100 by 100 world box.
		shared_ptr<ge::Mesh> worldBoxMesh = ge::pgeom::box(glm::vec3(0, 0, 0), glm::vec3(50, 50, 50));
		// Flip the normals to face in not out.
		{
			auto& norms = worldBoxMesh->normals;
			for (auto p = norms.begin(); p != norms.end(); ++p) {
				*p = -*p;
			}
		}
		// Reverse the triangles
		{
			auto& triangles = worldBoxMesh->triangles;
			for (auto p = triangles.begin(); p != triangles.end(); p += 3) {
				int tmp = *p;
				*(p + 0) = *(p + 1);
				*(p + 1) = tmp;
			}
		}

		shared_ptr<ge::MeshR> worldBoxMeshRenderer(new ge::MeshR());
		worldBoxMeshRenderer->setMesh(worldBoxMesh);
		worldBoxMeshRenderer->setTransform(worldBoxMeshRenderer->getTransform().setRotation(glm::quat(sqrt(0.5f), sqrt(0.5f), 0, 0)));
		worldBoxMeshRenderer->setMaterial(ge::Material(ge::ShaderProgram::find("diffuse")));

		// Set the lighting uniforms.
		worldBoxMeshRenderer->getMaterial().setColor("matDiffuse", glm::vec4(0.8f, 0.8f, 0.0f, 1.0f));
		worldBoxMeshRenderer->getMaterial().setColor("matAmbient", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

		worldBoxMeshRenderer->getMaterial().setColor("lightDiffuse", lightDiffuse);
		worldBoxMeshRenderer->getMaterial().setColor("lightAmbient", lightAmbient);

		// TODO: Apply transform here or in the motion state?
		btCollisionShape* collisionShape = createBvhTriangleMeshShape(worldBoxMesh);
		// new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		// This should get the transform from the mesh renderer.

		// For the world box.
		btDefaultMotionState* motionState =
			new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

		// For the quad.
		//btDefaultMotionState* motionState =
		//	new btDefaultMotionState(btTransform(btQuaternion(sqrt(0.5f), 0, 0, sqrt(0.5f)), btVector3(0, 0, 0)));

		btRigidBody::btRigidBodyConstructionInfo
			groundRigidBodyCI(0, motionState, collisionShape, btVector3(0, 0, 0));
		btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

		artemis::Entity & groundEntity = em->create();
		groundEntity.addComponent(new RenderComponent(worldBoxMeshRenderer));
		groundEntity.addComponent(new PhysicsComponent(collisionShape, groundRigidBody, COLL_WORLD, COLL_BOX));
		groundEntity.setGroup("WORLD");
		groundEntity.refresh();

		collisionShape->setUserPointer(&groundEntity);
	}

	// Create the player.
	{
		shared_ptr<ge::MeshR> meshR(new ge::MeshR());
		meshR->setMesh(ge::pgeom::box(glm::vec3(0, 0, 0), 0.5, 1, 0.5));

		meshR->setMaterial(ge::Material(ge::ShaderProgram::find("diffuse")));

		// Set the lighting uniforms.
		meshR->getMaterial().setColor("matDiffuse", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
		meshR->getMaterial().setColor("matAmbient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
		glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		meshR->getMaterial().setColor("lightDiffuse", lightDiffuse);
		meshR->getMaterial().setColor("lightAmbient", lightAmbient);

		glm::vec3 position = glm::vec3(0, 20, 0);
		meshR->setTransform(meshR->getTransform().setTranslation(position));

		btConvexShape* playerShape = new btCapsuleShape(0.25, 1);
		btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
		ghostObject->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 20, 0)));
		world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		ghostObject->setCollisionShape(playerShape);
		ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		btKinematicCharacterController* controller = new btKinematicCharacterController(ghostObject, playerShape, 0.5);
		controller->setWalkDirection(btVector3(0.0001f, 0, 0));

		// TODO: This should be handled by the physics system.
		world->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
		world->addAction(controller);

		artemis::Entity & player = em->create();
		player.addComponent(new RenderComponent(meshR));
		player.addComponent(new PlayerComponent(controller));
		player.addComponent(new TransformComponent(position));
		player.refresh();
	}

#ifdef never
	{
		shared_ptr<ge::MeshR> meshR(new ge::MeshR());
		meshR->setMesh(createBoxMesh());

		meshR->setMaterial(ge::Material(ge::ShaderProgram::find("diffuse")));

		// Set the lighting uniforms.
		meshR->getMaterial().setColor("matDiffuse", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
		meshR->getMaterial().setColor("matAmbient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		glm::vec4 lightDiffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
		glm::vec4 lightAmbient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		meshR->getMaterial().setColor("lightDiffuse", lightDiffuse);
		meshR->getMaterial().setColor("lightAmbient", lightAmbient);

		glm::vec3 position = glm::vec3(0, 10, 0);
		meshR->setTransform(meshR->getTransform().setTranslation(position).setScale(glm::vec3(5,5,5)));

		artemis::Entity & box = em->create();
		box.addComponent(new RotateComponent());
		box.addComponent(new TransformComponent(meshR->getTransform()));
		box.addComponent(new RenderComponent(meshR));
		box.setGroup("BOX");
		box.refresh();
	}
#endif
	
	// Octree stuff.
	OctreeAgentMesh octreeAgent;
	OctreeVisitorCamera visitor(&renderer);
	OctreeVisitorDraw visitorDraw(&renderer, ge::ShaderProgram::find("debug"));

	hxa7241_graphics::Octree<ge::MeshR> octree(hxa7241_graphics::Vector3r(-25.0f, -25.0f, -25.0f), 50.0f, 40, 8, 1.0f);
	//for (auto p = draw.begin(); p != draw.end(); ++p) {
	//	octree.insert(**p, octreeAgent);
	//}
	
	sf::Vector2i center;
	center.x = window.getSize().x / 2;
	center.y = window.getSize().y / 2;

	// Known seed for now.
	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	unsigned seed = 1234;
	std::mt19937 random(seed);
	float counter = 0.5 + ((float)random() / random.max()) * 0.5f;
	sf::Clock clock;
	while (window.isOpen())
	{
		float time = clock.restart().asSeconds();
		world->stepSimulation(time, 10);
		aWorld.loopStart();
		aWorld.setDelta(time);
		physicsSystem.process();
		rotateSystem.process();
		playerSystem.process();
		collisionSystem.process();
		expirationSystem.process();
		
		//int byteSize;
		//int leafCount;
		//int itemRefCount;
		//int maxDepth;
		//octree.getInfo(byteSize, leafCount, itemRefCount, maxDepth);
		//printf("byteSize=%d leafCount=%d itemRefCount=%d maxDepth=%d\n", byteSize, leafCount, itemRefCount, maxDepth);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			switch (event.type) {
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				case sf::Keyboard::R:
					//doRotate = !doRotate;
					break;
				case sf::Keyboard::B:
					renderSystem.setShowBounds(!renderSystem.getShowBounds());
					break;
				case sf::Keyboard::F:
					renderSystem.setWireframe(!renderSystem.getWireframe());
					break;
				}
				break;
			}
		}

		counter -= time;
		if (counter < 0) {
			counter = 0.1 + ((float)random() / random.max()) * 0.3f;
			float x = 1 + ((float)random() / random.max()) * 48.0f;
			float z = 1 + ((float)random() / random.max()) * 48.0f;
			artemis::Entity& box = createBox(ge::ShaderProgram::find("diffuse"), em, glm::vec3(x, 40, z));
			box.refresh();
		}

		renderer.clearBuffers(ge::Renderer::BUFFER_COLOR_AND_DEPTH);

		window.clear();

		cameraControl->update(time);

		camera.update();
		
		renderSystem.process();

		window.display();

		// TODO: Horrific.
		//if (appInFocus(&window)) {
		//	sf::Mouse::setPosition(center, window);
		//}
	}

	return 0;
}
