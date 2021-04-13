/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
attribute vec4 normal;
attribute vec4 color;		
attribute vec2 uv;

attribute vec4 jointIndex;
attribute vec4 jointWeight;
uniform mat4 matrixPalette[64];

uniform mat4 modelViewMatrix;
uniform mat4 worldViewProjectionMatrix;
uniform mat4 normalMatrix;

varying vec3 v_normal;
varying vec4 v_eye;
varying vec2 v_uv;

void main() {
	mat4 mat = matrixPalette[int(jointIndex.x)] * jointWeight.x
               + matrixPalette[int(jointIndex.y)] * jointWeight.y
               + matrixPalette[int(jointIndex.z)] * jointWeight.z
               + matrixPalette[int(jointIndex.w)] * jointWeight.w;

    mat4 nmat = mat;
    nmat[3] = vec4(0, 0, 0, 1);

    vec4 skinnedNormal = nmat * vec4(normal.xyz, 1.0);

    vec4 blendPos = mat * position;

	gl_Position = worldViewProjectionMatrix * vec4(blendPos.xyz, 1);

	v_normal = normalize(vec3(normalMatrix * skinnedNormal));
	// TODO: Perhaps normalize eye coordinate here?
	v_eye = -(modelViewMatrix * blendPos);
	v_uv = uv;
}