/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
uniform mat4 worldViewProjectionMatrix;
void main() {
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);
}