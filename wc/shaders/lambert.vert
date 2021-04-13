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

uniform vec4 l_direction;
uniform vec4 l_diffuse;
uniform vec4 l_ambient;

uniform vec4 m_diffuse;
uniform vec4 m_ambient;
uniform vec4 m_emissive;

varying vec4 v_color;
varying vec2 v_uv;

void main() {
	gl_Position = worldViewProjectionMatrix * vec4(position.xyz, 1);

	vec3 viewNormal = normalize(vec3(normalMatrix * normal));
	vec3 lightDir = normalize(vec3(l_direction));
	float NdotL = max(dot(viewNormal, lightDir), 0.0);

	v_color = m_emissive + l_ambient * m_ambient + (l_diffuse * m_diffuse) * NdotL;
	v_uv = uv;
}