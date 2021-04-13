/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
attribute vec4 normal;

uniform mat4 modelViewMatrix;
uniform mat4 worldViewProjectionMatrix;
uniform mat4 normalMatrix;

varying vec3 pos_eye;
varying vec3 n_eye;

void main() {
    pos_eye = vec3 (modelViewMatrix * vec4 (position.xyz, 1.0));
    n_eye = vec3 (modelViewMatrix * vec4 (normal.xyz, 0.0));
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);
}