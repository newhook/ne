/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;

uniform mat4 worldViewProjectionMatrix;

varying vec3 outTexture0;

void main() {
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);

    outTexture0 = position.xyz;
}