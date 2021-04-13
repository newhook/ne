/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/RenderQueue.h>
#include <ge/Camera.h>
#include <ge/Renderable.h>
#include <ge/Renderer.h>
#include <ge/MeshR.h>
#include <ge/Material.h>
#include <ge/TextureSettings.h>

#include <algorithm>

using namespace ge;
using namespace std;
using namespace glm;

//static b3d::Logger logger("com.bittrap.b3d.RenderQueue", __FILE__);

namespace {
	/**
	* Calculates the distance from a spatial to the camera. Distance is a squared distance.
	*
	* @param spat
	*            Renderable to check distance.
	* @return Distance from Renderable to current context's camera.
	*/
	float distanceToCam(Camera* cam, Renderable* spat) {
		// this optimization should not be stored in the spatial
		// if (spat.queueDistance != float.NEGATIVE_INFINITY) {
		// return spat.queueDistance;
		// }

		// spat.queueDistance = 0;

		vec3 spatPosition = spat->getPosition();
#ifndef NDEBUG
		//if (!glmutil::isValid(spatPosition)) {
		//	return MathUtils::NEGATIVE_INFINITY;
		//}
#endif

		// TODO: Distance to cam?
		vec3 tmp = spatPosition - cam->getTransform().getTranslation();
		return abs(dot(tmp, cam->getTransform().getForward()));
	}

	int compareByStates(const pair<size_t, Renderable*>& m1, const pair<size_t, Renderable*>& m2) {
		const ShaderProgram* s1 = m1.second->getMaterial(m1.first).getShader();
		const ShaderProgram* s2 = m2.second->getMaterial(m2.first).getShader();
		if (s1 == s2) {
#ifdef never
			const TextureSettings& ts1 = m1->getMaterial()->getTextureSettings();
			const TextureSettings& ts2 = m2->getMaterial()->getTextureSettings();

			if (ts1 == ts2) {
				return 0;
			}
			else if (!ts1 && ts2) {
				return -1;
			}
			else if (!ts2 && ts1) {
				return 1;
			}

			TextureKey* keys1[MAX_TEXTURE_UNITS];
			ts1->getTextureKeys(keys1);
			TextureKey* keys2[MAX_TEXTURE_UNITS];
			ts2->getTextureKeys(keys2);

			for (int i = 0; i < MAX_TEXTURE_UNITS; ++i) {
				TextureKey* key1 = keys1[i];
				TextureKey* key2 = keys2[i];

				if (key1 == key2) {
					continue;
				}

				if (key1 < key2) {
					return -1;
				}
				return 1;
			}

#endif
			return 0;
		}
		if (s1 < s2) {
			return -1;
		}
		return 1;
	}

	class OpaqueCompare {
	public:

		OpaqueCompare() {
		}

		bool operator()(const pair<size_t, Renderable*>& s1, const pair<size_t, Renderable*>& s2) {
			if (s1 == s2) {
				return false;
			}
			int val = compareByStates(s1, s2);
			if (val > 0) {
				return true;
			}
			else if (val < 0) {
				return false;
			}

			return distanceToCam(s1.second->getCamera(), s1.second) < distanceToCam(s2.second->getCamera(), s2.second);
		}
	};

	bool orthoCompare(const pair<size_t, Renderable*>& s1, const pair<size_t, Renderable*>& s2) {
		//if (s2->getOrthoOrder() <= s1->getOrthoOrder()) {
		//	return false;
		//}
		//else {
			return true;
		//}
	}


	float lengthSquared(const glm::vec3& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	float distanceSquared(const glm::vec3& v1, const glm::vec3& v2) {
		return lengthSquared(v1 - v2);
	}

	class TransparentCompare {
	public:

		TransparentCompare() {

		}

		bool operator()(const pair<size_t, Renderable*>& s1, const pair<size_t, Renderable*>& s2) {
			const Material& mat1 = s1.second->getMaterial(s1.first);
			const Material& mat2 = s2.second->getMaterial(s2.first);
			//if (mat1 && mat2) {
			//	BlendOrderHint hint1 = mat1->getRenderSettings()->getBlendOrderHint();
			//	BlendOrderHint hint2 = mat2->getRenderSettings()->getBlendOrderHint();
			//	if (hint1 != hint2) {
			//		return hint1 < hint2;
			//	}
			//}

			//int sortHint1 = mat1->getActiveTechnique()->getRenderSettings()->getSortOrderHint();
			//int sortHint2 = mat2->getActiveTechnique()->getRenderSettings()->getSortOrderHint();
			//if (sortHint1 != sortHint2) {
			//	return sortHint1 < sortHint2;
			//}

			// This calculation treats the camera as a plane
			float ds1 = distanceToCam(s1.second->getCamera(), s1.second);
			float ds2 = distanceToCam(s2.second->getCamera(), s2.second);

			vec3 pos1 = s1.second->getPosition();
			vec3 pos2 = s2.second->getPosition();

			float epsilon = 0.001f;
			if (abs(ds1 - ds2) > epsilon) {
				return ds1 < ds2;
			}

			if (distanceSquared(pos1, pos2) > epsilon) {
				if (abs(pos1.y - pos2.y) > epsilon) {
					return pos1.y > pos2.y;
				}

				if (abs(pos1.x - pos2.x) > epsilon) {
					return pos1.x > pos2.x;
				}
				if (abs(pos1.z - pos2.z) > epsilon) {
					return pos1.z > pos2.z;
				}
			}

			return s1 < s2;
		}
	};
}

RenderQueue::RenderQueue(int index, Renderer* r) : _index(index), _renderer(r) {
}

RenderQueue::~RenderQueue() {

}

void
RenderQueue::add(size_t submesh, Renderable* r) {
	assert(r);
	_currentList.push_back(make_pair(submesh, r));
}

void
RenderQueue::clear() {
	_currentList.clear();
}

void
RenderQueue::render() const {
	for (auto p : _currentList) {
		p.second->render(p.first, _renderer);
	}
}

// Get rid of this probably.
OrthoRenderQueue::OrthoRenderQueue(int index, Renderer* r) : RenderQueue(index, r) {
}

void
OrthoRenderQueue::sort() {
	std::sort(_currentList.begin(), _currentList.end(), orthoCompare);
}

OpaqueRenderQueue::OpaqueRenderQueue(int index, Renderer* r) : RenderQueue(index, r) {
}

void
OpaqueRenderQueue::sort() {
	if (!_currentList.empty()) {
		OpaqueCompare opaqueCompare;
		//    LOG_T("OpaqueRenderQueue::sort %d elements", _currentList.size());
		std::sort(_currentList.begin(), _currentList.end(), opaqueCompare);
	}
}

TransparentRenderQueue::TransparentRenderQueue(int index, Renderer* r) : RenderQueue(index, r) {
}


void
TransparentRenderQueue::sort() {
	if (!_currentList.empty()) {
		TransparentCompare transparentCompare;
		std::sort(_currentList.begin(), _currentList.end(), transparentCompare);
	}
}
