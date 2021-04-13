/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#ifdef NE_TARGET_WINDOWS
#   include <GL/glew.h>
#endif

#ifdef NE_TARGET_OSX
#   include <OpenGL/OpenGL.h>
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <OpenGL/glext.h>
#endif
