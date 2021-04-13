/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
attribute vec4 normal;
attribute vec4 color;		
attribute vec2 uv;

uniform mat4 modelViewMatrix;
uniform mat4 worldViewProjectionMatrix;
uniform mat4 normalMatrix;

varying vec3 v_normal;
varying vec4 v_eye;
varying vec2 v_uv;

void main() {
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);

	v_normal = normalize(vec3(normalMatrix * normal));
	// TODO: Perhaps normalize eye coordinate here?
	v_eye = -(modelViewMatrix * position);
	v_uv = uv;
}