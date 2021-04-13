/**
 *
 * Copyright (c) 2014 Matthew Newhook. All rights reserved.
 *
 */

#pragma once

#ifdef __APPLE__
#   include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC
#   if TARGET_OS_IPHONE
#       define NE_TARGET_IPHONE
#   else
#       define NE_TARGET_OSX
#   endif
#elif defined(_MSC_VER)
#   define NE_TARGET_WINDOWS
#endif

#ifdef NE_TARGET_WINDOWS
#   define _CRT_SECURE_NO_WARNINGS
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#endif

// Headers to precompile.
#include <ge/gl.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/epsilon.hpp>
#include <glm/gtx/quaternion.hpp>
