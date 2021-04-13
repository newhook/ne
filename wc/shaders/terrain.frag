/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

uniform sampler2D heightmap;
uniform sampler2D terrain;

varying vec4 outColor;
varying vec2 outTexture0;

void main() {
	//gl_FragColor = outColor;
	gl_FragColor = outColor * texture2D(terrain, outTexture0);
}