/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
attribute vec2 uv;

uniform mat4 worldViewProjectionMatrix;

// uv of frame 0.
uniform vec2 f0;

// uv of frame 1.
uniform vec2 f1;

// Width of the fb texture.
uniform float width;

// Blend ratio between frame 0 and 1.
uniform float ratio;

// The actual frame uv.
varying vec2 v_fuv0;
varying vec2 v_fuv1;

void main() {
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);

    v_fuv0 = uv / width + f0;
	v_fuv1 = uv / width + f1;
}