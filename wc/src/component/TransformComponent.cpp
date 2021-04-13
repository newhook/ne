/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <Config.h>
#include <component/TransformComponent.h>

using namespace wc;

ge::Transform
TransformComponent::getWorldTransform(size_t i) const {
	if (localTransform.size() > i) {
		ge::Transform t = localTransform[i];
		return t.combineWithParent(transform);
	}
	return transform;
}

ge::Transform
TransformComponent::getWorldTransform() const {
	if (localTransform.size() > 0) {
		// TODO: Main component? Perhaps average?
		ge::Transform t = localTransform[0];
		return t.combineWithParent(transform);
	}
	return transform;
}