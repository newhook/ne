/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
uniform sampler2D imgTexture0;
uniform vec4 lightDirection;	

varying vec2 outTexture0;

varying vec4 diffuse;
varying vec4 ambient;
varying vec3 outNormal;
varying vec3 halfVector;

void main() {
	vec3 lightDir = normalize(vec3(lightDirection));
 
    vec4 color = ambient;
    vec3 n = normalize(outNormal);
 
    float NdotL = max(dot(n, lightDir),0.0);
	if (NdotL > 0.0) {
        color += diffuse * NdotL;
        vec3 halfV = normalize(halfVector);
        float NdotHV = max(dot(n, halfV), 0.0);

		// No specular for now.
        //color += gl_FrontMaterial.specular *
        //        gl_LightSource[0].specular *
        //        pow(NdotHV, gl_FrontMaterial.shininess);
    }
	gl_FragColor = color;
	//gl_FragColor = outColor * texture2D(imgTexture0, outTexture0);
}