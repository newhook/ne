/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
attribute vec4 normal;
attribute vec4 color;		
attribute vec2 uv;	

uniform mat4 worldViewProjectionMatrix;
uniform mat4 normalMatrix;
uniform mat4 viewMatrix; // Camera model view matrix.

uniform vec4 lightDirection;	
uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;

uniform vec4 matDiffuse;
uniform vec4 matAmbient;

varying vec2 outTexture0;	

varying vec4 diffuse;
varying vec4 ambient;
varying vec3 outNormal;
varying vec3 halfVector;

void main() {
	gl_Position = worldViewProjectionMatrix * position;	

	vec4 ecPosition = viewMatrix * vec4(position.xyz, 1.0);
	vec3 VP = vec3(lightDirection - ecPosition);
	VP = normalize(VP);
	vec3 eye = vec3(0.0, 0.0, 1.0);

	halfVector = normalize(VP + eye);

	outNormal = normalize(vec3(normalMatrix * normal));

    diffuse = matDiffuse;// * lightDiffuse;
    ambient = matAmbient;// * lightAmbient;
    
	outTexture0 = uv;
}