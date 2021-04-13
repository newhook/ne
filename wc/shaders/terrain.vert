/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

attribute vec4 position;
attribute vec2 uv;

uniform mat4 worldViewProjectionMatrix;
uniform mat4 normalMatrix;
uniform mat4 viewMatrix; // Camera model view matrix.

// Lighting uniforms.
uniform vec4 l_direction;
uniform vec4 l_diffuse;
uniform vec4 l_ambient;

uniform vec4 m_diffuse;
uniform vec4 m_ambient;

//
// The world space position of the center of the mesh.
//
uniform vec3 center;

uniform sampler2D heightmap;

uniform float width;
uniform float localTexelToWorld;
uniform float texelSize;
uniform float texelAspect;

varying vec2 outTexture0;
varying vec4 outColor;

vec3 filterNormal(vec2 uv) 
{ 
	vec4 h; 
	h[0] = texture2D(heightmap, uv + texelSize*vec2( 0,-1)).r * texelAspect; 
	h[1] = texture2D(heightmap, uv + texelSize*vec2(-1, 0)).r * texelAspect; 
	h[2] = texture2D(heightmap, uv + texelSize*vec2( 1, 0)).r * texelAspect; 
	h[3] = texture2D(heightmap, uv + texelSize*vec2( 0, 1)).r * texelAspect; 
	vec3 n; 
	n.z = h[0] - h[3]; 
	n.x = h[1] - h[2]; 
	n.y = 2.0;
 
	return normalize(n); 
}

void main() {
	vec2 worldOrigin = (width / 2.0) * texelSize - center.xz * texelSize;
	vec2 worlduv = worldOrigin + uv * localTexelToWorld;

	vec4 pos = position;
	pos.y = texture2D(heightmap, worlduv).r * texelAspect;

	gl_Position = worldViewProjectionMatrix * vec4(pos.xyz, 1);

	vec3 normal = filterNormal(worlduv);

	outTexture0 = worlduv;

	//
	// Lighting.
	//	
	vec3 viewNormal = normalize(vec3(normalMatrix * vec4(normal.xyz, 1)));
	vec3 lightDir = normalize(vec3(l_direction));
	float NdotL = max(dot(viewNormal, lightDir), 0.0);
	outColor = m_ambient + m_diffuse * NdotL;

	//outColor = viewNormal;
}