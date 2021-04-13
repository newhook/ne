/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
varying vec3 outTexture0;
uniform samplerCube imgTexture0;

void main() {
	gl_FragColor = textureCube(imgTexture0, outTexture0);
}