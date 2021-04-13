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

uniform vec4 lightDirection;
uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;

uniform vec4 matDiffuse;
uniform vec4 matAmbient;

varying vec4 outColor;
varying vec2 outTexture0;	

void main() {
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);

	vec3 viewNormal = normalize(vec3(normalMatrix * normal));
	vec3 lightDir = normalize(vec3(lightDirection));
	float NdotL = max(dot(viewNormal, lightDir), 0.0);
	outColor = matAmbient + matDiffuse * NdotL;
	//outColor = vec4(viewNormal, 1.0);

	outTexture0 = uv;
}