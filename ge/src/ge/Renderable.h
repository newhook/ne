/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/Transform.h>

#include <vector>

namespace ge {
class MeshR;
class Renderer;
class Bounds;
class Material;
class Camera;
class Renderable {
protected:

	Camera* _camera;
public:

	Renderable();
	virtual ~Renderable();

	void setCamera(Camera* camera) {
		_camera = camera;
	}
	Camera* getCamera() {
		return _camera;
	}

	virtual void setName(const std::string& name) = 0;
	virtual const std::string& getName() const = 0;

	virtual size_t getNumSubmeshes() const = 0;

	// Get the bounds in world space.
	virtual const Bounds& getBounds() const = 0;

	// TODO: Replace with getTransform()?
	virtual glm::vec3 getPosition() const = 0;

	virtual void setMaterials(const std::vector<Material>& materials) = 0;

	virtual void setMaterial(const Material& material) = 0;
	virtual Material& getMaterial(size_t submesh = 0) = 0;

	// The transform of the renderable in world space.
	virtual Transform getTransform() const = 0;
	virtual void setTransform(const Transform& transform) = 0;

	// For external use.
	virtual void draw(Renderer* renderer) = 0;
	virtual void render(size_t submesh, Renderer* renderer) = 0;
};

}