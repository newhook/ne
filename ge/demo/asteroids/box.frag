/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
varying vec4 outColor;
varying vec2 outTexture0;
uniform sampler2D imgTexture0;

void main() {
	gl_FragColor = outColor;
	//gl_FragColor = outColor * texture2D(imgTexture0, outTexture0);
}