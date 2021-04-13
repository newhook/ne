/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
uniform sampler2D diffuseTexture;

uniform vec4 l_direction;
uniform vec4 l_diffuse;
uniform vec4 l_ambient;

uniform vec4 m_diffuse;
uniform vec4 m_ambient;
uniform vec4 m_emissive;
uniform vec4 m_specular;
uniform float m_shininess;

varying vec3 v_normal;
varying vec4 v_eye;
varying vec2 v_uv;

void main() {
	// set the specular term to black
    vec4 spec = vec4(0.0);
 
    // normalize both input vectors
    vec3 n = normalize(v_normal);
	// TODO: Do in .vert?
    vec3 e = normalize(vec3(v_eye));
 
    float intensity = max(dot(n, l_direction), 0.0);
 
    // if the vertex is lit compute the specular color
    if (intensity > 0.0) {
        // compute the half vector
        vec3 h = normalize(l_direction + e);  
        // compute the specular term into spec
        float intSpec = max(dot(h, n), 0.0);
        spec = m_specular * pow(intSpec, m_shininess);
    }

	// TODO: Doesn't take into account the lights color ambient/diffuse.
	gl_FragColor = max(intensity * m_diffuse + spec, m_ambient) * texture2D(diffuseTexture, v_uv);
}