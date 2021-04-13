/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <vector>

namespace ge {
class Camera;
class Renderer;
class Renderable;
class RenderQueue {
protected:
	int _index;
	Renderer* _renderer;
	std::vector<std::pair<size_t, Renderable*> > _currentList;

public:

	RenderQueue(int index, Renderer* r);
	virtual ~RenderQueue();

	int index() const {
		return _index;
	}
	void add(size_t submesh, Renderable* spatial);
	void clear();

	virtual void render() const;
	virtual void sort() = 0;
};

class OrthoRenderQueue : public RenderQueue {
public:

	OrthoRenderQueue(int index, Renderer* r);

	virtual void sort();
};

class OpaqueRenderQueue : public RenderQueue {
public:
	OpaqueRenderQueue(int index, Renderer* r);

	virtual void sort();
};

class TransparentRenderQueue : public RenderQueue {
public:
	TransparentRenderQueue(int index, Renderer* r);
	virtual void sort();
};

}