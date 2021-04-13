/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
varying vec4 v_color;
varying vec2 v_uv;
uniform sampler2D diffuseTexture;

void main() {
	gl_FragColor = v_color * texture2D(diffuseTexture, v_uv);
}