/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
uniform sampler2D diffuseTexture;

uniform float ratio;

varying vec2 v_fuv0;
varying vec2 v_fuv1;

void main() {
	gl_FragColor = (1.0 - ratio) * texture2D(diffuseTexture, v_fuv0) + ratio * texture2D(diffuseTexture, v_fuv1);
}