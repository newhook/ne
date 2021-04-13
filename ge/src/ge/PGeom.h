/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <memory>

namespace ge {
class Mesh;
namespace pgeom
{

std::shared_ptr<ge::Mesh> box(const glm::vec3& center, const glm::vec3& extents);
std::shared_ptr<ge::Mesh> box(const glm::vec3 &, float, float, float);
std::shared_ptr<ge::Mesh> gridPlane(float horizontalExtend, float verticalExtend, int rows, int columns);
// Create a quad oriented in the x, y plane.
std::shared_ptr<ge::Mesh> quad(float w, float h);
std::shared_ptr<ge::Mesh> quadXZ(float w, float h);
std::shared_ptr<ge::Mesh> sphere(int zSamples, int radialSamples, float radius, bool viewInside = false);

}
}